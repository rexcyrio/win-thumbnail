# win-thumbnail

`win-thumbnail` is a Node.js addon that uses native Windows APIs to retrieve the thumbnail of a file. If the file does not have a thumbnail, `win-thumbnail` will return a high resolution file icon image of the file as fallback.

I created this Node.js addon as I wanted larger file icon images (as fallback) than what Electron's [`app.getFileIcon()`](https://www.electronjs.org/docs/latest/api/app#appgetfileiconpath-options) could provide.

I use this addon in my other project [Purpl](https://github.com/rexcyrio/purpl-electron-react-vite).

## Usage

```js
// path to the binding.js file
const winThumbnail = require("../dist/binding.js"); 

// the first parameter is the absolute path to the file
// the second parameter is a positive integer representing the desired size (in pixels) of the PNG image returned
// => returns a base64 data url string representing the thumbnail of the `demo.mp4` file
const dataUrl = winThumbnail.create("C:\\Users\\John Doe\\demo.mp4", 64);
```

## Behaviour

- If the file does not exist, the addon will throw a JavaScript `TypeError`
- If the file has a thumbnail, the addon will return the file thumbnail
- If the file does not have a thumbnail, the addon will return its file icon of the same desired size

## Future Improvements

Allow the user to decide whether to

- Prefer thumbnail to file icon (current behaviour)
- Request thumbnail only
- Request file icon only
