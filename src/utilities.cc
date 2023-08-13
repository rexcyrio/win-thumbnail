#include <string>
#include <shlobj_core.h>
#include <shobjidl_core.h>

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