## 加密壓縮
tar -czvf - 2021_final_project | openssl des3 -salt -k <password> -out 2021_final_project.tar.gz

## 解密壓縮
openssl des3 -d -k <password> -salt -in 2021_final_project.tar.gz | tar xzvf -