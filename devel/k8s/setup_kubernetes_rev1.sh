#!/bin/sh

CRIO_VERSION=1.15

is_installed() {
	kubectl get nodes 2>&1 >/dev/null
}

update_registry_server() {
	# update registry server address in /etc/hosts
	# if you want to remove the entry, simply don't provide the ip address.
	local server_name='registry.tmaxos.com'
	local server_regex='^.*registry\.tmaxos\.com$'
	local server_ip=$1

	local file='/etc/hosts'
	if [ -z "$server_ip" ]
	then # ip not provided, remove the entry.
		sudo sed -i "/$server_regex/d" $file
	else
		if ! grep $server_regex $file 2>&1 >/dev/null
		then
			printf "$server_ip $server_name\n" | sudo tee -a $file >/dev/null
		else
			sudo sed -i "s:$server_regex:$server_ip $server_name:" $file
		fi
	fi
}

install_crio() {
	# install the package
	sudo apt install cri-o-$CRIO_VERSION --yes --install-suggests

	# fix the config
	sudo sed -i 's:^conmon = .*$:conmon = "/usr/bin/conmon":' /etc/crio/crio.conf

	# add registries
	sudo tee /etc/containers/registries.d/tmaxos.yaml >/dev/null <<EOF
docker:
    registry.tmaxos.com:
        sigstore: https://registry.tmaxos.com
EOF

	# add modules
	sudo tee /etc/modules-load.d/crio.conf >/dev/null <<EOF
overlay
br_netfilter
EOF
	sudo systemctl restart systemd-modules-load

	# add sysctl config
	sudo tee /etc/sysctl.d/crio.conf >/dev/null <<EOF
net.bridge.bridge-nf-call-ip6tables = 1
net.bridge.bridge-nf-call-iptables = 1
net.bridge.bridge-nf-call-arptables = 1
net.ipv4.ip_forward = 1
EOF
	sudo sysctl --system
}

remove_crio() {
	sudo systemctl stop crio
	sudo rm /etc/sysctl.d/crio.conf /etc/modules-load.d/crio.conf /etc/containers/registries.d/tmaxos.yaml
	sudo apt purge cri-o-$CRIO_VERSION --yes
	sudo apt purge containernetworking-plugins --yes
}

install_kubernetes() {
	# install the package
	sudo apt install kubeadm --yes
	sudo systemctl stop kubelet

	# configure kubeadm to run with CRI-O
	printf 'KUBELET_EXTRA_ARGS=--feature-gates="AllAlpha=false,RunAsGroup=true" --container-runtime=remote --cgroup-driver=systemd --container-runtime-endpoint="unix:///var/run/crio/crio.sock" --runtime-request-timeout=5m --fail-swap-on=false\n' | sudo tee /etc/default/kubelet >/dev/null

	# add unit dependencies
	sudo tee /etc/systemd/system/kubelet.service.d/20-crio.conf >/dev/null <<EOF
[Unit]
Requires=crio.service
After=crio.service
EOF
	sudo systemctl daemon-reload

	# init the master node
	sudo systemctl start kubelet
	sudo kubeadm init # --pod-network-cidr=10.244.0.0/16

	# set the certificate path
	if ! env | grep '^KUBECONFIG=' 2>&1 >/dev/null
	then
		certfile=/etc/kubernetes/admin.conf
		mkdir -p $HOME/.kube
		sudo cat $certfile | tee $HOME/.kube/config >/dev/null
	fi

	# untaint the master node
	kubectl taint nodes --all node-role.kubernetes.io/master-
}

remove_kubernetes() {
	sudo kubeadm reset --force
	sudo systemctl stop kubelet
	rm -r $HOME/.kube
	sudo rm /etc/systemd/system/kubelet.service.d/20-crio.conf /etc/default/kubelet
	sudo systemctl daemon-reload
	sudo apt purge kubeadm kubelet kubectl --yes
}

# arg parse (fix this)
case "$1" in
	--force)
		force=1
		;;
	--remove)
		remove=1
		;;
	*)
		;;
esac

if [ -n "$remove" ]
then
	update_registry_server
	remove_kubernetes
	remove_crio
else
	update_registry_server 192.168.8.12
	if is_installed
	then
		if [ -z "$force" ]
		then
			printf "kubernetes is working already.\n"
			exit 1
		else
			remove_kubernetes
			remove_crio
		fi
	fi

	install_crio
	install_kubernetes
fi
