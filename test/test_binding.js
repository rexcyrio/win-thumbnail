const winThumbnail = require("../dist/binding.js");
const assert = require("assert");

const GOOD_FILE_PATH = "C:\\Users\\Stefan Lee\\battery1.html";
const BAD_FILE_PATH_1 = "C:\\Users\\jacob";
const BAD_FILE_PATH_2 = "";

const GOOD_THUMBNAIL_SIZE = 100;
const BAD_THUMBNAIL_SIZE_1 = -1;
const BAD_THUMBNAIL_SIZE_2 = 0;
const BAD_THUMBNAIL_SIZE_3 = Number.MAX_SAFE_INTEGER;

const EXPECTED_DATA_URL_OF_SCOOP_PS1 =
  "data:image/png;base64,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";

assert.ok(winThumbnail);

function testNumberOfArguments() {
  assert.throws(() => {
    winThumbnail.create();
  }, TypeError);

  assert.throws(() => {
    winThumbnail.create(GOOD_FILE_PATH);
  }, TypeError);

  assert.throws(() => {
    winThumbnail.create(GOOD_FILE_PATH, GOOD_THUMBNAIL_SIZE, "abc");
  }, TypeError);
}

function testFilePath() {
  assert.doesNotThrow(() => {
    winThumbnail.create(GOOD_FILE_PATH, GOOD_THUMBNAIL_SIZE);
  }, TypeError);

  assert.throws(() => {
    winThumbnail.create(BAD_FILE_PATH_1, GOOD_THUMBNAIL_SIZE);
  }, TypeError);

  assert.throws(() => {
    winThumbnail.create(BAD_FILE_PATH_2, GOOD_THUMBNAIL_SIZE);
  }, TypeError);
}

function testThumbnailSize() {
  assert.doesNotThrow(() => {
    winThumbnail.create(GOOD_FILE_PATH, GOOD_THUMBNAIL_SIZE);
  }, TypeError);

  assert.throws(() => {
    winThumbnail.create(GOOD_FILE_PATH, BAD_THUMBNAIL_SIZE_1);
  }, TypeError);

  assert.throws(() => {
    winThumbnail.create(GOOD_FILE_PATH, BAD_THUMBNAIL_SIZE_2);
  }, TypeError);

  assert.throws(() => {
    winThumbnail.create(GOOD_FILE_PATH, BAD_THUMBNAIL_SIZE_3);
  }, TypeError);
}

function testReturnValue() {
  const dataUrl = winThumbnail.create("C:\\Users\\Stefan Lee\\scoop.ps1", 40);
  assert.strictEqual(dataUrl, EXPECTED_DATA_URL_OF_SCOOP_PS1);
}

try {
  testNumberOfArguments();
  testFilePath();
  testThumbnailSize();
  testReturnValue();
  console.log("All tests passed.");
} catch (error) {
  console.log("FAILED");
  console.error(error);
}
