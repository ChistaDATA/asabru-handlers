//
// Created by Midhun Darvin on 22/03/24.
//

#include "SendFileCommand.h"

extern "C" SendFileCommand *createSendFileCommand()
{
    return new SendFileCommand;
}

extern "C" void destroySendFileCommand(SendFileCommand *c)
{
    delete c;
}

bool SendFileCommand::Execute(ComputationContext *context) {
    auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
    std::string request_path = request->uri().path();
    std::cout << "SendFileCommand :" << request_path << std::endl;
    auto response = simple_http_server::HttpResponse(simple_http_server::HttpStatusCode::Ok);
    std::string content = "";

    if (request_path == "/") {
        request_path = "/index.html";
    }

    std::string publicFolderPath = std::getenv("PUBLIC_FOLDER_PATH");
    std::string file_path = publicFolderPath + "/" + request_path;
    std::ifstream myfile(file_path);

    std::string temp;
    if (myfile.is_open()) { // always check whether the file is open
        while (myfile) // equivalent to myfile.good()
        {
            std::getline(myfile, temp);
            content += temp;
            content += "\n";
        }
    }

    std::string file_ext = file_path.substr(file_path.find_last_of(".") + 1);
    std::string mime_type;
    if (file_ext == "js") {
        mime_type = "text/javascript";
    } else if (mime_type == "html") {
        mime_type = "text/html";
    } else if (mime_type == "css") {
        mime_type = "text/css";
    }
    response.SetHeader("Content-Type", mime_type);
    response.SetContent(content);
    context->Put("response", response);
    return true;
}
