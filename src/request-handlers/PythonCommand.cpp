#include "PythonCommand.h"
#include "util/PythonInterpreter.h"
#include <nlohmann/json.hpp>

extern "C" PythonCommand *createPythonCommand() {
    return new PythonCommand;
}

extern "C" void destroyPythonCommand(PythonCommand *c) {
    delete c;
}

bool PythonCommand::Execute(ComputationContext *context) {
    auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
    try {
        /**
         * Find python file and execute it.
         */
        PythonInterpreter pythonInterpreter;

        ComputationContext *pythonComputationContext = new ComputationContext;
        nlohmann::json body = nlohmann::json::parse(request->content());
        std::string pythonCommandParameters = body["inputParams"].dump();
        std::unordered_map<std::string, std::string> params;
        params["inputParams"] = pythonCommandParameters;
        pythonComputationContext->Put("params", params);

        pythonInterpreter.Execute("multiply", pythonComputationContext);
        auto content = std::any_cast<std::string>(pythonComputationContext->Get("response"));

        auto *response = new simple_http_server::HttpResponse(simple_http_server::HttpStatusCode::Ok);
        response->SetHeader("Content-Type", "text/plain");
        response->SetContent(content);
        context->Put("response", response);

        return true;
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return false;
    }

}
