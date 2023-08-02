const winthumbnail = require("../dist/binding.js");
const assert = require("assert");

assert.ok(winthumbnail);

const result = winthumbnail.create("C:\\Users\\Stefan Lee");
assert.strictEqual(result, "C:\\Users\\Stefan Lee");

assert.throws(() => {
  winthumbnail.create();
});

assert.throws(() => {
  winthumbnail.create(1, 2);
});

assert.throws(() => {
  winthumbnail.create("C:\\Users\\Stefan Lee\\dummy");
});

console.log("OK");
