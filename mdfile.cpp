#include "mdfile.h"
//Отправка сообщений
void msgsend(int work_sock, string mess){
    char *buffer = new char[4096];
    strcpy(buffer, mess.c_str());
    send(work_sock, buffer, mess.length(), 0);
}

//Комстрока
string* Server::comstr(int argc, char *argv[]) {
    string* com = new string[4];
    if(argc == 1){
    cout << "Kalculator"  << endl;
    cout << "-h info" << endl;
    cout << "-f file name" << endl;
    cout << "-p port" << endl;
    cout << "-e error file" << endl;
    exit(0);
    }
    int opt;
    int port = 33333;
    string file_name = "/etc/vcalc.conf"; // Исправлен путь
    string file_error = "/var/log/vcalc.log";
    string error;
    com[3] = error;

    while ((opt = getopt(argc, argv, "hf:p:e:")) != -1) {
        switch(opt) {
            case 'h':
                cout << "Векторный калькулятор выполняющий действие произведения вектора"  << endl;
                cout << "-f БД пользователей -p порт -e файл ошибок" << endl;
                cout << "-h info" << endl;
                cout << "-f file name" << endl;
                cout << "-p port" << endl;
                cout << "-e error file" << endl;
                exit(0);
                break;
            case 'f':
                file_name = string(optarg);
                com[0] = file_name;
                break;
            case 'p':
                port = stoi(string(optarg));
                com[1] = to_string(port);
                break;
            case 'e':
                file_error = string(optarg);
                com[2] = file_error;
                break;
        }
    }
    return com;
}

//MD5
string MD(string sah){
Weak::MD5 hash;
    string digest;
    StringSource(sah, true,  new HashFilter(hash, new HexEncoder(new StringSink(digest))));  // строка-приемник
      return digest;
 }

//Ошибки
void errors(string error, string file_error){
    ofstream file;
    file.open(file_error, ios::app);
    if(!file.is_open()){
        throw error_server(string(error));
    }
    if(file.is_open()){
        time_t seconds = time(NULL);
        tm* timeinfo = localtime(&seconds);
        file << error << ':'<<asctime(timeinfo)<<endl;
        file.close();
        cout << error <<':'<<asctime(timeinfo)<<endl;
    }
}



//self_addr
int Server::self_addr(string error, string file_error, int port){
            int s = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in * self_addr = new (sockaddr_in);
            self_addr->sin_family = AF_INET;
            self_addr->sin_port = htons(port);
            self_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
            cout << "Wait for connect client...\n";
        int b = bind(s,(const sockaddr*) self_addr,sizeof(sockaddr_in));
            
            
            if(b == -1) {
                string error = "binding error";
                throw error_server(string(error));
                errors(error,  file_error);
                return 2;
            }


            listen(s, SOMAXCONN);
            return s;
            }



//Client_addr
int Server::client_addr(int s, string error,  string file_error){
                sockaddr_in * client_addr = new sockaddr_in;
                socklen_t len = sizeof (sockaddr_in);
                int work_sock = accept(s, (sockaddr*)(client_addr), &len);
                
                if(work_sock == -1) {
                string error = "error #2";
                throw error_server(string(error));
                errors(error, file_error);
                return 0;
                }
                else {
                    //Успешное подключение к серверу
                    cout << "Successfull client connection!\n";
                    return work_sock;
                }
                }


//Авторизация
int autorized(int work_sock, string file_name, string file_error){

                    string ok = "OK";
                    string salt = "RISERISERISERISE";
                    string err = "ERR";
                    string error;
                    char msg[255];

                    //Авторизация
                    recv(work_sock, &msg, sizeof(msg), 0);
                    string message = msg;
                        string login, hashq;

                        fstream file;
                        file.open(file_name);
                        if(!file.is_open()){
                            cout << "Ошибка открытия БД" <<endl;
                                error = "Ошибка открытия БД";
                                errors(error, file_error);
                                throw error_server(string(error));
                        }else{
                        getline (file, login, ':');
                        getline (file, hashq);

                    //СВЕРКА ЛОГИНОВ
                    if(message != login){
                        msgsend(work_sock,  err);
                            error = "Ошибка логина";
                            errors(error, file_error);
                            throw error_server(string(error));
                            close(work_sock);
                        return 1;
                    }else{
                    //соль отправленная клиенту
                    msgsend(work_sock,  salt);
                    recv(work_sock, msg, sizeof(msg), 0);
                    string sah = salt + hashq;
                    string digest;
                    digest = MD(sah);
                    //СВЕРКА ПАРОЛЕЙ
                    if(digest != msg){
                        cout << digest << endl;
                        cout << msg << endl;
                        msgsend(work_sock,  err);
                            error = "Ошибка пароля";
                            errors(error, file_error);
                            throw error_server(string(error));
                        close(work_sock);
                        return 1;
                    }else{
                        msgsend(work_sock,  ok);
                    }
}
return 1;
}}

//Сумма
    int math(int work_sock){
                    uint32_t count;
                    uint32_t numb;
                    float vect;
                        recv(work_sock, &count, sizeof(count), 0);
                    //цикл векторов
                    for(int j=0; j<count; j++){
                        recv(work_sock, &numb, sizeof(numb), 0);//прием длинны для первого вектора
                        float sum = 0;
                    //цикл значений
                    for(int i=0; i<numb; i++){
                        recv(work_sock, &vect, sizeof(vect), 0);
                        sum = sum+vect;
                    }
                    float mfc;
                    mfc = sum;
                    
                    send(work_sock, &mfc, sizeof(mfc), 0);
                    }
                    cout << "Program finish!" <<endl;
                    close(work_sock);
                    return 1;
                    }


