#include <napi.h>

Napi::String Method(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  size_t number_of_arguments = info.Length();

  if (number_of_arguments != 1)
  {
    std::string error_message = "Expected 1 argument. Received " + std::to_string(number_of_arguments) + " argument(s)";

    Napi::TypeError::New(env, error_message)
        .ThrowAsJavaScriptException();

    return Napi::String::New(env, "");
  }

  Napi::String path_string = info[0].As<Napi::String>();
  return path_string;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "create"),
              Napi::Function::New(env, Method));
  return exports;
}

NODE_API_MODULE(winthumbnail, Init)
