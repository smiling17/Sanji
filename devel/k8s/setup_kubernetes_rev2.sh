#!/bin/sh

CRIO_VERSION=1.15

is_installed() {
	which kubectl >/dev/null && kubectl get nodes 2>&1 >/dev/null
}

update_registry_server_ip() {
	# update registry server address in /etc/hosts
	local server_name='registry.tmaxos.com'
	local server_regex='^.*registry\.tmaxos\.com$'
	local server_ip=$1

	local file='/etc/hosts'
	if [ -z "$server_ip" ]
	then
		printf "update_registry_server_ip: ip not provided\n"
		exit 1
	else
		if ! grep $server_regex $file 2>&1 >/dev/null
		then
			printf "$server_ip $server_name\n" | sudo tee -a $file >/dev/null
		else
			sudo sed -i "s:$server_regex:$server_ip $server_name:" $file
		fi
	fi
}

remove_registry_server_ip() {
	local server_regex='^.*registry\.tmaxos\.com$'
	local file='/etc/hosts'
	sudo sed -i "/$server_regex/d" $file
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
        sigstore: http://registry.tmaxos.com
    docker.io:
	sigstore: http://docker.io
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

disable_swap() {
	for unit in `cd /run/systemd/generator;ls *.swap`
	do
		sudo systemctl stop $unit
		sudo systemctl mask $unit
		sudo systemctl daemon-reload
	done
}

restore_swap() {
	for unit in `cd /run/systemd/generator;ls *.swap`
	do
		local mask_cand=/etc/systemd/system/$unit
		if readlink $mask_cand | grep /dev/null >/dev/null
		then
			sudo rm $mask_cand
			sudo systemctl daemon-reload
			sudo systemctl start $unit
		fi
	done
}

install_kubernetes() {
	# install the package
	sudo apt install kubeadm --yes
	sudo systemctl stop kubelet

	# configure kubeadm to run with CRI-O
	printf 'KUBELET_EXTRA_ARGS=--feature-gates="AllAlpha=false,RunAsGroup=true" --container-runtime=remote --cgroup-driver=systemd --container-runtime-endpoint="unix:///var/run/crio/crio.sock" --runtime-request-timeout=5m\n' | sudo tee /etc/default/kubelet >/dev/null

	# add unit dependencies
	sudo tee /etc/systemd/system/kubelet.service.d/20-crio.conf >/dev/null <<EOF
[Unit]
Requires=crio.service
After=crio.service
EOF
	sudo systemctl daemon-reload

	# init the master node
	sudo systemctl start kubelet
	sudo kubeadm init --v=5 # --pod-network-cidr=10.244.0.0/16

	# set the certificate path
	if ! env | grep '^KUBECONFIG=' 2>&1 >/dev/null
	then
		local cert_from=/etc/kubernetes/admin.conf
		local cert_to=$HOME/.kube/config
		mkdir -p $HOME/.kube
		sudo cp $cert_from $cert_to
		sudo chown $USER. $cert_to
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
	--remove)
		remove=1
		;;
	*)
		;;
esac

if [ -n "$remove" ]
then
	remove_kubernetes
	remove_crio
	restore_swap
	remove_registry_server_ip
else
	if is_installed
	then
		printf "kubernetes is working already.\n"
		exit 1
	else
		/bin/sh setup_kubernetes.sh --remove

		update_registry_server_ip 192.168.8.12
		disable_swap
		install_crio
		install_kubernetes
	fi
fi
