//
// Created by Midhun Darvin on 08/11/23.
//

#include "SendHtmlCommand.h"

extern "C" SendHtmlCommand *createSendHtmlCommand()
{
    return new SendHtmlCommand;
}

extern "C" void destroySendHtmlCommand(SendHtmlCommand *c)
{
    delete c;
}

bool SendHtmlCommand::Execute(ComputationContext *context) {
//    auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
    auto *response = new simple_http_server::HttpResponse(simple_http_server::HttpStatusCode::Ok);
    std::string content = "";

    std::string publicFolderPath = std::getenv("PUBLIC_FOLDER_PATH");
    std::string file_path = publicFolderPath + "/index.html";
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
    response->SetHeader("Content-Type", "text/html");
    response->SetContent(content);
    context->Put("response", response);
    return true;
}
