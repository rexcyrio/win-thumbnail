const winthumbnail = require("../dist/binding.js");
const assert = require("assert");

assert(winthumbnail, "The expected function is undefined");

function testBasic()
{
    const result =  winthumbnail("hello");
    assert.strictEqual(result, "world", "Unexpected value returned");
}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");

console.log("Tests passed- everything looks OK!");