#include <napi.h>
#include "utilities.cc"

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

  Napi::String first_argument = info[0].ToString();
  std::string path_string = first_argument.Utf8Value();
  const char *path_string_ptr = path_string.c_str();
  struct stat metadata;

  if (stat(path_string_ptr, &metadata) != 0)
  {
    std::string error_message = path_string + " is an invalid file path";

    Napi::TypeError::New(env, error_message)
        .ThrowAsJavaScriptException();

    return Napi::String::New(env, "");
  }

  HBITMAP hbitmap = GetThumbnail(L"C:\\Users\\Stefan Lee\\Downloads\\xeuphoria.jpg", 400);
  convertHBitmapToCharBuffer(hbitmap);

  return first_argument;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "create"),
              Napi::Function::New(env, Method));
  return exports;
}

NODE_API_MODULE(winthumbnail, Init)
