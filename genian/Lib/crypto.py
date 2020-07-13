# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import tempfile
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))


class Crypte:
    CIPHER_SUITE = "-aes-128-cbc"

    INIT_VECTOR = "31323334353637383930313233343536"

    RANDOM_GEN_DATA = "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"

    CRYPT_KEY_IDX = [34, 275, 10, 76, 10, 8, 10, 95, 10, 35, 10, 8, 76, 110, 34,
                     8, 34, 35, 34, 143, 76, 110, 34, 11, 10, 76, 275, 95, 76, 110, 143, 35]
    CRYPT_KEY_IDX = [34, 275, 10, 76, 10, 8, 10, 95, 10, 35, 10, 8, 76, 110, 34,
                     8, 34, 35, 34, 143, 76, 110, 34, 11, 10, 76, 275, 95, 76, 110, 143, 35]


def decryption(file_path):
    """
    파일을 복호화하여 파일 내용을 리스트로 리턴.
    :return: file list
    """
    try:
        """
        파일 존재여부 확인
        """
        fp = open(file_path, 'r')
        fp.close()

        """ 리눅스 배포판 별로 사용하는 임시폴더(/tmp) 명을 리턴 함. """
        tmp_path = tempfile.gettempdir()

        """ 파일 명만 추출 """
        file_path_token = file_path.split(".")[0].split("/")
        file_path_token.reverse()
        file_name = file_path_token[0]

        """ ubuntu : /tmp/파일명.trn """
        FILE_TRUNC = ''.join([tmp_path, "/", file_name, ".trn"])

        """
        앞쪽에 28바이트로 작성된 signature 제거한 임시 파일 생성
        """
        command = ''.join(["dd if='", file_path, "' bs=28 skip=1 of='", FILE_TRUNC, "' 2> /dev/null"])

        popen = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        result_popen = list(popen.communicate())

        enc = Crypte()
        crypte_key = "".join([enc.RANDOM_GEN_DATA[idx] for idx in enc.CRYPT_KEY_IDX])

        """
        임시 파일에서 복호화
        """
        command = ''.join(["openssl enc ", enc.CIPHER_SUITE,
                           " -d -K ", crypte_key,
                           " -iv ", enc.INIT_VECTOR,
                           " -in ", FILE_TRUNC,
                           " 2> /dev/null"])

        popen = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        result_popen = list(popen.communicate())

        fileinfolist = result_popen[0].replace("\x00", '')

        del crypte_key
        del enc

        """
        임시 파일 삭제
        """
        command = ''.join(["rm -f ", FILE_TRUNC])
        popen = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        result_popen = list(popen.communicate())
        result = fileinfolist.splitlines()

        return result
    except Exception as msg:
        print("Exception ERRMSG=" + str(msg))
