#include <napi.h>
#include "utilities.cc"

Napi::String Method(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  size_t number_of_arguments = info.Length();

  if (number_of_arguments != 2)
  {
    std::string error_message = "Expected 2 arguments. Received " + std::to_string(number_of_arguments) + " argument(s)";

    Napi::TypeError::New(env, error_message)
        .ThrowAsJavaScriptException();

    return Napi::String::New(env, "");
  }

  std::string first_argument = info[0].ToString().Utf8Value();
  std::string second_argument = info[1].ToString().Utf8Value();

  std::string &path_string = first_argument;
  int thumbnail_size;

  try
  {
    thumbnail_size = std::stoi(second_argument);
  }
  catch (const std::invalid_argument &e)
  {
    std::string invalid_argument_error_message(e.what());
    std::string error_message = invalid_argument_error_message + "Expected second argument to be a positive integer. Received " + second_argument + " instead";

    Napi::TypeError::New(env, error_message)
        .ThrowAsJavaScriptException();

    return Napi::String::New(env, "");
  }
  catch (const std::out_of_range &e)
  {
    std::string out_of_range_error_message(e.what());
    std::string error_message = out_of_range_error_message + "Expected second argument to be a positive integer. Received " + second_argument + " instead";

    Napi::TypeError::New(env, error_message)
        .ThrowAsJavaScriptException();

    return Napi::String::New(env, "");
  }

  if (thumbnail_size <= 0)
  {
    std::string error_message = "Expected second argument to be a positive integer. Received " + second_argument + " instead";

    Napi::TypeError::New(env, error_message)
        .ThrowAsJavaScriptException();

    return Napi::String::New(env, "");
  }

  const char *path_string_ptr = path_string.c_str();
  struct stat metadata;

  if (stat(path_string_ptr, &metadata) != 0)
  {
    std::string error_message = path_string + " is an invalid file path";

    Napi::TypeError::New(env, error_message)
        .ThrowAsJavaScriptException();

    return Napi::String::New(env, "");
  }

  std::wstring path_wstring(path_string.begin(), path_string.end());

  bool doesThumbnailRequireFlipping = false;
  HBITMAP hbitmap = GetFileThumbnail(path_wstring, thumbnail_size, &doesThumbnailRequireFlipping);

  std::string dataUrl = convertHBitmapToDataUrl(hbitmap, &doesThumbnailRequireFlipping);

  return Napi::String::New(env, dataUrl);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "create"),
              Napi::Function::New(env, Method));
  return exports;
}

NODE_API_MODULE(winthumbnail, Init)
