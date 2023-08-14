const winthumbnail = require("../dist/binding.js");
const assert = require("assert");

function main() {
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
    winthumbnail.create("");
  });

  assert.throws(() => {
    winthumbnail.create("C:\\Users\\Stefan Lee\\dummy");
  });
}

try {
  main();
  console.log("All tests passed.");
} catch (error) {
  console.log("FAILED");
  console.error(error);
}
