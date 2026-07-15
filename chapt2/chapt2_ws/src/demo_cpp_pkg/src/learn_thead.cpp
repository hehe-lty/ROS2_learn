#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <cpp-httplib/httplib.h>

class Download
{

public:
    void download(const std::string &host, const std::string &path,
                  const std::function<void(const std::string &, const std::string &)> &callback_word_count)
    {
        std::cout << "线程" <<std::this_thread::get_id()<< std::endl;
       httplib::Client client(host);
       auto response = client.Get(path);
       if(response && response->status == 200){
        callback_word_count(path, response->body);
       }
    }


    void start_download(const std::string &host, const std::string &path,
                  const std::function<void(const std::string &, const std::string &)> &callback_word_count)
    {
        auto download_fun= std::bind(&Download::download, this, std::placeholders::_1, 
        std::placeholders::_2, std::placeholders::_3);
        std::thread thread(download_fun, host, path, callback_word_count);
        thread.detach();
       
    }
};

int main()
{ 
    auto d = Download();
    auto word_count = [](const std::string &path, const std::string &result)->void {
        std::cout << "下载完成: " << path<< result.length()<< "->"<<result.substr(0,16)<<std::endl;
    };

    d.start_download("http://0.0.0.0:8000", "/chapt2_ws/novel1.txt", word_count);
    d.start_download("http://0.0.0.0:8000", "/chapt2_ws/novel2.txt", word_count);
    d.start_download("http://0.0.0.0:8000", "/chapt2_ws/novel3.txt", word_count);
            
    std::this_thread::sleep_for(std::chrono::milliseconds(1000*10));//10s

    return 0; 
   

}
