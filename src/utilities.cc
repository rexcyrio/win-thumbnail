#include <string>
#include <shlobj_core.h>
#include <shobjidl_core.h>
#include <gdiplus.h>
#include <vector>

// taken from https://cplusplus.com/forum/windows/100661/
HBITMAP GetThumbnail(std::wstring File)
{
  std::wstring Folder, FileName;
  int Pos = File.find_last_of(L"\\");
  Folder = File.substr(0, Pos);
  FileName = File.substr(Pos + 1);

  IShellFolder *pDesktop = NULL;
  IShellFolder *pSub = NULL;
  IExtractImage *pIExtract = NULL;
  LPITEMIDLIST pidl = NULL;

  HRESULT hr;
  hr = SHGetDesktopFolder(&pDesktop);
  if (FAILED(hr))
    return NULL;
  hr = pDesktop->ParseDisplayName(NULL, NULL, (LPWSTR)Folder.c_str(), NULL, &pidl, NULL);
  if (FAILED(hr))
    return NULL;
  hr = pDesktop->BindToObject(pidl, NULL, IID_IShellFolder, (void **)&pSub);
  if (FAILED(hr))
    return NULL;
  hr = pSub->ParseDisplayName(NULL, NULL, (LPWSTR)FileName.c_str(), NULL, &pidl, NULL);
  if (FAILED(hr))
    return NULL;
  hr = pSub->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&pidl, IID_IExtractImage, NULL, (void **)&pIExtract);
  if (FAILED(hr))
    return NULL;

  SIZE size;
  size.cx = 300;
  size.cy = 300;

  DWORD dwFlags = IEIFLAG_ORIGSIZE | IEIFLAG_QUALITY;

  HBITMAP hThumbnail = NULL;

  // Set up the options for the image
  OLECHAR pathBuffer[MAX_PATH];
  hr = pIExtract->GetLocation(pathBuffer, MAX_PATH, NULL, &size, 32, &dwFlags);

  // Get the image
  hr = pIExtract->Extract(&hThumbnail);

  pDesktop->Release();
  pSub->Release();

  return hThumbnail;
}

// taken from https://stackoverflow.com/a/51388079
void convertHBitmapToCharBuffer(HBITMAP hbitmap)
{
  // taken from https://learn.microsoft.com/en-us/windows/win32/api/gdiplusinit/nf-gdiplusinit-gdiplusstartup
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  // get gdi+ bitmap
  Gdiplus::Bitmap bitmap(hbitmap, nullptr);

  // write to IStream
  IStream *istream = nullptr;
  HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &istream);
  CLSID clsid_png;
  CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &clsid_png);
  bitmap.Save(istream, &clsid_png);

  // CLSID of PNG encoder is {557CF406-1A04-11D3-9A73-0000F81EF32E}
  // reference: https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-retrieving-the-class-identifier-for-an-encoder-use

  // uncomment to save thumbnail image to file
  // bitmap.Save(L"C:\\Users\\Stefan Lee\\Desktop\\test.png", &clsid_png);

  // get memory handle associated with istream
  HGLOBAL hg = NULL;
  GetHGlobalFromStream(istream, &hg);

  // copy IStream to buffer
  int bufsize = GlobalSize(hg);
  char *buffer = new char[bufsize];

  // lock & unlock memory
  LPVOID ptr = GlobalLock(hg);
  memcpy(buffer, ptr, bufsize);
  GlobalUnlock(hg);

  // release will automatically free the memory allocated in CreateStreamOnHGlobal
  istream->Release();

  Gdiplus::GdiplusShutdown(gdiplusToken);

  // PNG image data is now available in the variable `buffer` with size `bufsize`
}

// not used
// taken from https://stackoverflow.com/a/5346026
HRESULT GetGdiplusEncoderClsid(const std::wstring &format, GUID *pGuid)
{
  HRESULT hr = S_OK;
  UINT nEncoders = 0; // number of image encoders
  UINT nSize = 0;     // size of the image encoder array in bytes
  std::vector<BYTE> spData;
  Gdiplus::ImageCodecInfo *pImageCodecInfo = NULL;
  Gdiplus::Status status;
  bool found = false;

  if (format.empty() || !pGuid)
  {
    hr = E_INVALIDARG;
  }

  if (SUCCEEDED(hr))
  {
    *pGuid = GUID_NULL;
    status = Gdiplus::GetImageEncodersSize(&nEncoders, &nSize);

    if ((status != Gdiplus::Ok) || (nSize == 0))
    {
      hr = E_FAIL;
    }
  }

  if (SUCCEEDED(hr))
  {

    spData.resize(nSize);
    pImageCodecInfo = (Gdiplus::ImageCodecInfo *)&spData.front();
    status = Gdiplus::GetImageEncoders(nEncoders, nSize, pImageCodecInfo);

    if (status != Gdiplus::Ok)
    {
      hr = E_FAIL;
    }
  }

  if (SUCCEEDED(hr))
  {
    for (UINT j = 0; j < nEncoders && !found; j++)
    {
      if (pImageCodecInfo[j].MimeType == format)
      {
        *pGuid = pImageCodecInfo[j].Clsid;
        found = true;
      }
    }

    hr = found ? S_OK : E_FAIL;
  }

  return hr;
}