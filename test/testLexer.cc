#include <gtest/gtest.h>

#include <memory>

class LexerTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F( LexerTest, check_init ) {
  ASSERT_TRUE( true );
}

/*
R"(var counter = 0)"
R"(float pi = 3.14_15_926)"
R"(string msg = "Escaped "quotes" and \ slashes")"
R"(char newline = '\n')"
R"(bool is_ready = true)"
R"(def calculate(x: int, y: int) -> int:
ret x + y
done)"
R"(if a >= 10 and b <= 20:
a += 1
elseif a != b or not c:
b -= 1
else:
a = 2
b /= 2
)"
R"(while true do
var arr = [1, 2, 3] ++ [4, 5]
arr = arr -- [2]
var size = $arr
var filtered = arr ?
var reversed = arr @
if size > 10 break
continue
done)"
R"(try
throw exception
catch
cast_to int
must copy
finally
ret
done)"
R"(# This is a full line comment)"
R"(var x = 10 # This is an inline comment)"
R"()"
R"(    )"
R"(hidden_var)"
R"(123_bad_ident)"
R"(1_000__000)"
R"(1_000)"
R"(_1_000)"
R"(12.34.56)"
R"(12.34)"
R"(12.34)"
R"(.99)"
R"(99.)"
R"("Unclosed string)"
R"("Bad \escape")"
R"('')"
R"('ab')"
R"('\k')"
R"(!===)"
R"(===>)"
R"(->->)"
R"(+=-==/=)"
R"(&|^~`)"
R"(/* block comment? */)"
R"($?@%+++---)"
*/