def preExecute(context):
    return True

def execute(context):
    print("Will compute", context["key1"], "times", context["key2"])
    a = int(context["key1"])
    b = int(context["key2"])
    c = 0
    for i in range(0, a):
        c = c + b
    print(c)
    context["result"] = str(c)
    return True

def postExecute(context):
    return True
