//
// Created by Midhun Darvin on 08/11/23.
//

#include "HttpFileUploadCommand.h"

// Mutex for thread safety
std::mutex coutMutex;

extern "C" HttpFileUploadCommand *createHttpFileUploadCommand()
{
    return new HttpFileUploadCommand;
}

extern "C" void destroyHttpFileUploadCommand(HttpFileUploadCommand *c)
{
    delete c;
}

bool HttpFileUploadCommand::Execute(ComputationContext *context) {
    auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));

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

    size_t lpos = 0, rpos = 0;
    std::string request_content = request->content();

    std::cout << std::endl;
    rpos = request_content.find("\r\n\r\n", lpos);
    unsigned long content_length = std::stoul(request->header("Content-Length"));
    if (rpos == std::string::npos)
    {
        throw std::invalid_argument("Could not find request start line");
    }

    std::string file_name;
    std::string file_metadata = request_content.substr(lpos, rpos - lpos);
    int fileNameIndex = file_metadata.find("filename=");
    if (fileNameIndex != std::string::npos) {
        int i = fileNameIndex + 10;
        while (file_metadata.at(i) != '"') {
            file_name += file_metadata[i];
            i++;
        }
    }
    std::string file_content = request_content.substr(rpos + 4, content_length);

    std::cout << file_name << std::endl;
    std::cout << "content_length : " << content_length << std::endl;
    std::cout << "file_content_length : " << file_content.size() << std::endl;
    std::cout << "Request content 2 :" << std::endl;
    // Save the received file
    {
        // Get download folder path
        std::string downloads_folder_path = ".";
        if (std::getenv("DOWNLOADS_FOLDER")) {
            downloads_folder_path = std::getenv("DOWNLOADS_FOLDER");
        }
        std::string output_file_path = downloads_folder_path + "/" + file_name;
        std::vector<char> fileData(file_content.begin(), file_content.end());
        std::lock_guard<std::mutex> lock(coutMutex);
        std::ofstream outputFile(output_file_path, std::ios::out | std::ios::binary);
        outputFile.write(fileData.data(), file_content.size());
        std::cout << "File uploaded successfully.\n";
    }

    response->SetHeader("Content-Type", "text/html");
    response->SetHeader("Connection", "close");
    response->SetContent(content);
    context->Put("response", response);
    return true;
}
