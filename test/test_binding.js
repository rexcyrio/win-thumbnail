const winthumbnail = require("../dist/binding.js");
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

assert.ok(winthumbnail);

function testNumberOfArguments() {
  assert.throws(() => {
    winthumbnail.create();
  }, TypeError);

  assert.throws(() => {
    winthumbnail.create(GOOD_FILE_PATH);
  }, TypeError);

  assert.throws(() => {
    winthumbnail.create(GOOD_FILE_PATH, GOOD_THUMBNAIL_SIZE, "abc");
  }, TypeError);
}

function testFilePath() {
  assert.doesNotThrow(() => {
    winthumbnail.create(GOOD_FILE_PATH, GOOD_THUMBNAIL_SIZE);
  }, TypeError);

  assert.throws(() => {
    winthumbnail.create(BAD_FILE_PATH_1, GOOD_THUMBNAIL_SIZE);
  }, TypeError);

  assert.throws(() => {
    winthumbnail.create(BAD_FILE_PATH_2, GOOD_THUMBNAIL_SIZE);
  }, TypeError);
}

function testThumbnailSize() {
  assert.doesNotThrow(() => {
    winthumbnail.create(GOOD_FILE_PATH, GOOD_THUMBNAIL_SIZE);
  }, TypeError);

  assert.throws(() => {
    winthumbnail.create(GOOD_FILE_PATH, BAD_THUMBNAIL_SIZE_1);
  }, TypeError);

  assert.throws(() => {
    winthumbnail.create(GOOD_FILE_PATH, BAD_THUMBNAIL_SIZE_2);
  }, TypeError);

  assert.throws(() => {
    winthumbnail.create(GOOD_FILE_PATH, BAD_THUMBNAIL_SIZE_3);
  }, TypeError);
}

function testReturnValue() {
  const dataUrl = winthumbnail.create("C:\\Users\\Stefan Lee\\scoop.ps1", 40);
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
