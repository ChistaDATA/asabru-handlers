import json
def preExecute(context):
    return True

def execute(context):
    print("Context", context["inputParams"])
    if context["inputParams"] is not None or context["inputParams"] != '':
        fields = json.loads(context["inputParams"])
    print(fields)
    print("Will compute", fields["key1"], "times", fields["key2"])
    a = int(fields["key1"])
    b = int(fields["key2"])
    c = 0
    for i in range(0, a):
        c = c + b
    print(c)
    context["response"] = str(c)
    return True

def postExecute(context):
    return True
