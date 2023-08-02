const winthumbnail = require("../dist/binding.js");
const assert = require("assert");

assert.ok(winthumbnail);

const result = winthumbnail.create("C:\\Users\\Stefan Lee\\another");
assert.strictEqual(result, "C:\\Users\\Stefan Lee\\another");

assert.throws(() => {
  winthumbnail.create();
});

assert.throws(() => {
  winthumbnail.create(1, 2);
});

console.log("OK");
