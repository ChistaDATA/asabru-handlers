#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"
#include "http/http_message.h"

class UpdateEndPointServiceCommand: BaseComputationCommand {
    bool Execute(ComputationContext* context) override;
};
