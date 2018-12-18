
static const char _afb_description_vshl[] =
    "{\"openapi\":\"3.0.0\",\"$schema\":\"http://iot.bzh/download/openapi/sch"
    "ema-3.0/default-schema.json\",\"info\":{\"description\":\"\",\"title\":\""
    "High Level Voice Service API\",\"version\":\"1.0\",\"x-binding-c-generat"
    "or\":{\"api\":\"vshl\",\"version\":3,\"prefix\":\"afv_\",\"postfix\":\"\""
    ",\"start\":null,\"onevent\":null,\"init\":\"init\",\"scope\":\"\",\"priv"
    "ate\":false,\"noconcurrency\":true}},\"servers\":[{\"url\":\"ws://{host}"
    ":{port}/api/monitor\",\"description\":\"TS caching binding\",\"variables"
    "\":{\"host\":{\"default\":\"localhost\"},\"port\":{\"default\":\"1234\"}"
    "},\"x-afb-events\":[{\"$ref\":\"#/components/schemas/afb-event\"}]}],\"c"
    "omponents\":{\"schemas\":{\"afb-reply\":{\"$ref\":\"#/components/schemas"
    "/afb-reply-v3\"},\"afb-event\":{\"$ref\":\"#/components/schemas/afb-even"
    "t-v3\"},\"afb-reply-v3\":{\"title\":\"Generic response.\",\"type\":\"obj"
    "ect\",\"required\":[\"jtype\",\"request\"],\"properties\":{\"jtype\":{\""
    "type\":\"string\",\"const\":\"afb-reply\"},\"request\":{\"type\":\"objec"
    "t\",\"required\":[\"status\"],\"properties\":{\"status\":{\"type\":\"str"
    "ing\"},\"info\":{\"type\":\"string\"},\"token\":{\"type\":\"string\"},\""
    "uuid\":{\"type\":\"string\"},\"reqid\":{\"type\":\"string\"}}},\"respons"
    "e\":{\"type\":\"object\"}}},\"afb-event-v3\":{\"type\":\"object\",\"requ"
    "ired\":[\"jtype\",\"event\"],\"properties\":{\"jtype\":{\"type\":\"strin"
    "g\",\"const\":\"afb-event\"},\"event\":{\"type\":\"string\"},\"data\":{\""
    "type\":\"object\"}}}},\"responses\":{\"200\":{\"description\":\"A comple"
    "x object array response\",\"content\":{\"application/json\":{\"schema\":"
    "{\"$ref\":\"#/components/schemas/afb-reply\"}}}}}}}";

static const struct afb_verb_v3 _afb_verbs_vshl[] = {
    {.verb = NULL, .callback = NULL, .auth = NULL, .info = NULL, .vcbdata = NULL, .session = 0, .glob = 0}};

int init(afb_api_t api);

const struct afb_binding_v3 afbBindingV3 = {.api = "vshl",
                                            .specification = _afb_description_vshl,
                                            .info = "",
                                            .verbs = _afb_verbs_vshl,
                                            .preinit = NULL,
                                            .init = init,
                                            .onevent = NULL,
                                            .userdata = NULL,
                                            .provide_class = NULL,
                                            .require_class = NULL,
                                            .require_api = NULL,
                                            .noconcurrency = 1};
