#include <string>
#include <shlobj_core.h>
#include <shobjidl_core.h>
#include <gdiplus.h>
#include <vector>
#include <iostream>
#include <combaseapi.h>
#include <objbase.h>
#include "base64encode.cc"
#include <memory>
#include <gdiplusimaging.h>

#pragma comment(lib, "Gdiplus.lib")

Gdiplus::Status HBitmapToBitmap(HBITMAP source, Gdiplus::PixelFormat pixel_format, Gdiplus::Bitmap **result_out, bool *doesThumbnailRequireFlipping);

HBITMAP GetFileThumbnail(std::wstring full_path, int thumbnail_size, bool *doesThumbnailRequireFlipping)
{
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

  IShellItemImageFactory *p_shell_item_image_factory = NULL;
  HRESULT hresult = NULL;

  hresult = SHCreateItemFromParsingName(full_path.c_str(), NULL, IID_IShellItemImageFactory, (void **)&p_shell_item_image_factory);
  if (FAILED(hresult) || !p_shell_item_image_factory)
  {
    std::cerr << "SHCreateItemFromParsingName ERROR: " << std::hex << hresult << std::endl;
    return NULL;
  }

  SIZE size;
  size.cx = thumbnail_size;
  size.cy = thumbnail_size;

  HBITMAP hbitmap = NULL;

  // reference: https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishellitemimagefactory-getimage

  // try `SIIGBF_THUMBNAILONLY` first
  hresult = p_shell_item_image_factory->GetImage(size, SIIGBF_RESIZETOFIT | SIIGBF_BIGGERSIZEOK | SIIGBF_THUMBNAILONLY, &hbitmap);
  if (FAILED(hresult) || !hbitmap)
  {
    // try `SIIGBF_ICONONLY` second
    hresult = p_shell_item_image_factory->GetImage(size, SIIGBF_RESIZETOFIT | SIIGBF_BIGGERSIZEOK | SIIGBF_ICONONLY, &hbitmap);
    if (FAILED(hresult) || !hbitmap)
    {
      std::cerr << "GetImage ERROR: " << std::hex << hresult << std::endl;
      return NULL;
    }

    // if `SIIGBF_ICONONLY` is used, the thumbnail produced requires flipping
    *doesThumbnailRequireFlipping = true;
  }

  p_shell_item_image_factory->Release();

  CoUninitialize();
  return hbitmap;
}

// taken from https://stackoverflow.com/a/51388079
std::string convertHBitmapToDataUrl(HBITMAP hbitmap, bool *doesThumbnailRequireFlipping)
{
  // taken from https://learn.microsoft.com/en-us/windows/win32/api/gdiplusinit/nf-gdiplusinit-gdiplusstartup
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  // get gdi+ bitmap
  // Gdiplus::Bitmap bitmap(hbitmap, nullptr);

  Gdiplus::Bitmap *p_bitmap_with_alpha_channel = NULL;

  Gdiplus::Status s = HBitmapToBitmap(hbitmap, PixelFormat32bppARGB, &p_bitmap_with_alpha_channel, doesThumbnailRequireFlipping);
  if (s != Gdiplus::Ok)
  {
    std::cerr << "HBitmapToBitmap ERROR: " << s << std::endl;
  }

  // write to IStream
  IStream *istream = nullptr;
  HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &istream);
  CLSID clsid_png;
  CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &clsid_png);
  (*p_bitmap_with_alpha_channel).Save(istream, &clsid_png);

  // CLSID of PNG encoder is {557CF406-1A04-11D3-9A73-0000F81EF32E}
  // reference: https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-retrieving-the-class-identifier-for-an-encoder-use

  // uncomment to save thumbnail image to file
  // (*p_bitmap_with_alpha_channel).Save(L"C:\\Users\\Stefan Lee\\Desktop\\test.png", &clsid_png);

  // get memory handle associated with istream
  HGLOBAL hg = NULL;
  GetHGlobalFromStream(istream, &hg);

  // copy IStream to buffer
  int bufsize = GlobalSize(hg);
  unsigned char *buffer = new unsigned char[bufsize];

  // lock & unlock memory
  LPVOID ptr = GlobalLock(hg);
  memcpy(buffer, ptr, bufsize);
  GlobalUnlock(hg);

  // release will automatically free the memory allocated in CreateStreamOnHGlobal
  istream->Release();

  // when uncommented, the JavaScript tests do not complete
  // Gdiplus::GdiplusShutdown(gdiplusToken);

  // PNG image data is now available in the variable `buffer` with size `bufsize`
  std::string base64_string = base64_encode(buffer, bufsize);
  return "data:image/png;base64," + base64_string;

  // clean up
  // delete[] buffer;
}

// taken from https://stackoverflow.com/a/5860094
Gdiplus::Status HBitmapToBitmap(HBITMAP source, Gdiplus::PixelFormat pixel_format, Gdiplus::Bitmap **result_out, bool *doesThumbnailRequireFlipping)
{
  BITMAP source_info = {0};
  if (!::GetObject(source, sizeof(source_info), &source_info))
    return Gdiplus::GenericError;

  Gdiplus::Status s;

  std::unique_ptr<Gdiplus::Bitmap> target(new Gdiplus::Bitmap(source_info.bmWidth, source_info.bmHeight, pixel_format));
  if (!target.get())
    return Gdiplus::OutOfMemory;

  s = target->GetLastStatus();
  if (s != Gdiplus::Ok)
    return s;

  Gdiplus::BitmapData target_info;
  Gdiplus::Rect rect(0, 0, source_info.bmWidth, source_info.bmHeight);

  s = target->LockBits(&rect, Gdiplus::ImageLockModeWrite, pixel_format, &target_info);
  if (s != Gdiplus::Ok)
    return s;

  if (target_info.Stride != source_info.bmWidthBytes)
    return Gdiplus::InvalidParameter; // pixel_format is wrong!

  CopyMemory(target_info.Scan0, source_info.bmBits, source_info.bmWidthBytes * source_info.bmHeight);

  s = target->UnlockBits(&target_info);
  if (s != Gdiplus::Ok)
    return s;

  if ((*doesThumbnailRequireFlipping) == true)
  {
    s = target->RotateFlip(Gdiplus::RotateFlipType::RotateNoneFlipY);
    if (s != Gdiplus::Ok)
      return s;
  }

  *result_out = target.release();

  return Gdiplus::Ok;
}
