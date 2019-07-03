#ifndef __JARVIS__HPP__
#define __JARVIS__HPP__


#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <json/json.h>
#include "include/base/http.h"
#include "include/speech.h"
#include "include/image_classify.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_map>


#define SPEECH_FILE "temp_file/demo.wav"
#define PLAY_FILE "temp_file/play.mp3"
#define CMD_ETC "command.etc"
#define IMAGE_FILE "image/test.jpg"


//连接到图灵机器人 智能对话
class InterRobot{
private:
    std::string url = "http://openapi.tuling123.com/openapi/api/v2";
    std::string api_key = "8d60c5a550ae4cc99fe4e82ff051c0e3";
    //std::string api_key = "26b3b70191b1493e840538412eabdc1e";
    std::string user_id = "1";
    aip::HttpClient client;

public:
    InterRobot(){

    }
    Json::Value PostRequest(Json::Value data)
    {
        std::string response;
        Json::Value obj;
        Json::CharReaderBuilder crbuilder;
        int code = this->client.post(url, nullptr, data, nullptr, &response);
        if(code != CURLcode::CURLE_OK){
            obj[aip::CURL_ERROR_CODE]=code;
            return obj;

        }
        std::string error;
        std::unique_ptr<Json::CharReader> reader(crbuilder.newCharReader());
        reader->parse(response.data(), response.data() + response.size(), &obj, &error);
        return obj;

    }
    std::string Talk(std::string &message)
    {
        Json::Value root;
        Json::Value item1;
        Json::Value item2;
        root["reqType"] = 0;
        item1["text"] = message;
        item2["inputText"] = item1;
        item1.clear();
        root["perception"] = item2;
        item2.clear();
        item2["apiKey"] = api_key;
        item2["userId"] = user_id;
        root["userInfo"] = item2;
        item2.clear();
        //std::cout << root.toStyledString() << std::endl;
        Json::Value ret = PostRequest(root);
        //std::cout << result.toStyledString() << std::endl;
        //Json::Value Intent = result["intent"];
        Json::Value _result = ret["results"];
        Json::Value values = _result[0]["values"];
        std::cout <<"Robot: "<< values["text"].asString() << std::endl;
        //for(auto i = 0; i < _result.size(); i++){
        //    Json::Value values = _result[i]["values"];
        //    std::cout <<"Robot: "<< values["text"].asString() << std::endl;
        //}
        //std::cout << result.toStyledString() << std::endl;
        return values["text"].asString();
    }
    ~InterRobot()
    {}
};
//百度AI图像处理

class ImageProcessing{
private:
    std::string app_id = "16703236";
    std::string api_key = "Kw4B3HZkFLpL9vE3TXKUcu5F";
    std::string secret_key = "C8SzocgxQa5Me8C14KhmaWBeZfhjDajp";
    aip::Imageclassify *client;
public:
    ImageProcessing()
    {
        client = new aip::Imageclassify(app_id, api_key, secret_key);
    }
    void ImageResult()
    {
        Json::Value result;
        std::string image;
        aip::get_file_content(IMAGE_FILE, &image);
        // 调用通用物体识别
        //result = client->advanced_general(image, aip::null);
        // 调用logo商标识别
        //result = client->logo_search(image, aip::null);
        //std::cout << result.toStyledString() << std::endl;

        // 如果有可选参数
        std::map<std::string, std::string> options;
        options["custom_lib"] = "false";
        // 带参数调用logo商标识别
        result = client->logo_search(image, options);
        std::cout << result.toStyledString() << std::endl;

    }
    ~ImageProcessing()
    {}
};



//百度AI语音识别
class SpeechRec{
private:
    //你注册完毕百度语音识别平台的账号后，在平台上就会有你的app_id
    std::string app_id="16684660";
    //你注册完毕百度语音识别平台的账号后，在平台上就会有你的api_key
    std::string api_key="15d8oiLPDGeoc3nGioqakVal";
    //你注册完毕百度语音识别平台的账号后，在平台上就会有你的secret_key
    std::string secret_key="fdakOLQ9EKoLh2vRCZlZWLo3pHGvyIX5";
    //SDK对应的语音识别客户端
    aip::Speech *client;
public:
    SpeechRec()
    {
        client = new aip::Speech(app_id, api_key, secret_key);
    }
    //语音识别Automatic Speech Recognition
    void ASR(int &err_code, std::string &message)
    {
        std::cout << std::endl << "...正在识别..." << std::endl;
        std::map<std::string, std::string> options; //以中文形式显示
        options["dev_pid"] = "1536";
        options["lan"] = "ZH";
        std::string file_content;
        aip::get_file_content(SPEECH_FILE, &file_content);//获取你的语音文件
        //开始识别，需要你的语音文件，格式是“wav”， 频率是16k
        Json::Value result = client->recognize(file_content, "wav", 16000, options);
        err_code = result["err_no"].asInt();
        if(err_code == 0){
			//std::cout<<"err_code == 0"<<std::endl;
            // std::cout << result.toStyledString() << std::endl;
            message = result["result"][0].asString();
            // std::cout << "message : " << message<< std::endl;
        }
        else{
            message="...识别错误...";
        }
        //std::cout << "语音识别结果: " << std::endl << result.toStyledString();
    }
    //语音合成Text To Speech
    void TTS(std::string message)
    {
        //std::cout << "TTS : " << message << std::endl;
        std::ofstream ofile;
        std::string file_ret;
        std::map<std::string, std::string> options;
        options["spd"] = "5";
        options["per"] = "0";
        ofile.open(PLAY_FILE, std::ios::out | std::ios::binary);
        //语音合成，将文本转成语音，放到指定目录，形成指定文件
        Json::Value result = client->text2audio(message, options, file_ret);
        if(!file_ret.empty()){
            ofile << file_ret;
        }
        else{
            std::cout << "error: " << result.toStyledString();
        }
        ofile.close();
    }
    ~SpeechRec()
    {
        delete client;
        client = NULL;
    }
};


class Jarvis{
private:
    InterRobot robot;
    SpeechRec sr;
    std::unordered_map<std::string, std::string> command_set;
public:
    Jarvis()
    {
        char buffer[256];
        std::ifstream in(CMD_ETC);
        if(!in.is_open()){
            std::cerr << "open file error" <<std::endl;
            exit(1);

        }
        std::string sep = ":";
        while(in.getline(buffer, sizeof(buffer))){
            std::string str = buffer;
            std::size_t pos = str.find(sep);
            if(std::string::npos == pos){
                std::cerr << "Load Etc Error" << std::endl;
                exit(2);

            }
            std::string k = str.substr(0, pos);
            std::string v = str.substr(pos+sep.size());
            k+="。";
            command_set.insert(std::make_pair(k, v));

        }
        std::cout << "Load command etc ... done" << std::endl;
        in.close();
    }
    bool Exec(std::string command, bool print)
    {
        FILE *fp = popen(command.c_str(), "r");
        if(NULL == fp){
            std::cerr << "popen error!" << std::endl;
            return false;
        }
        if(print){
            char c;
            std::cout << "--------------------start---------------------" << std::endl;
            while (fread(&c, 1, 1, fp) > 0){
                std::cout << c;
            }
            std::cout << std::endl;
            std::cout << "---------------------end----------------------" << std::endl;
        }
        pclose(fp);
        return true;
    }
    //判定消息是否是需要执行的命令，如果是命令，需要执行他，而不需要交给图灵机器人进行对话
    bool MessageIsCommand(std::string _message, std::string &cmd)
    {
        std::unordered_map<std::string, std::string>::iterator iter =
            command_set.find(_message);
        if(iter != command_set.end())
        {
            cmd = iter->second;
            return true;
        }
        cmd = "";
        return false;
    }
    //使用arecord工具进行录音，并进行语音识别，语音转文本
    bool RecordAndASR(std::string &message)
    {
        int err_code = -1;
        std::string record = "arecord -t wav -c 1 -r 16000 -d 5 -f S16_LE ";
        record += SPEECH_FILE;
        record += ">/dev/null 2>&1"; //不显示输出结果或者消息
        std::cout << "...请讲话...";
        fflush(stdout);
        if(Exec(record, false)){
            sr.ASR(err_code, message);
            if(err_code == 0){
                return true;
            }
            std::cout << "语音识别失败..." << std::endl;
        }
        else{
            std::cout << "录制失败..." << std::endl;
       }
        return false;
    }

    //使用百度语音合成接口，文本转语音，在使用cvlc进行本地播放
    bool TTSAndPlay(std::string message)
    {
        //cvlc命令行式的播放
        std::string play = "cvlc --play-and-exit ";//播放完毕退出：--play-and-exit
        play += PLAY_FILE;
        play += " >/dev/null 2>&1";
        sr.TTS(message); //语音识别
        Exec(play, false); //执行播放
        return true;
    }

    void run()
    {
        volatile bool quit = false;
        std::string message;
        while(!quit){
            message="";
            sleep(5);
            bool ret = RecordAndASR(message);
            //std::cout << "请输入：" << std::endl;
            //std::cin>>message;
            //bool ret = message.empty();
            if(ret){
                std::string cmd;
                std::cout << "life: " << message << std::endl;
                if(MessageIsCommand(message, cmd)){//判定是否是命令
                    std::cout << "start is command" << std::endl;
                    if(message == "退出。"){
                        TTSAndPlay("好的");
                        std::cout << "bye bye ... :)" << std::endl;
                        quit = true;
                        Exec(cmd,false);
                    }
                    else{
                        Exec(cmd, true);
                    }
                }
                else{ //不是命令，就交付给图灵机器人识别
                    std::string play_message = robot.Talk(message);
                    //std::cout << play_message << std::endl;
                    //sr.TTS(play_message);
					TTSAndPlay(play_message);
                }
			}
        }
    }
    ~Jarvis()
    {}

};

#endif


