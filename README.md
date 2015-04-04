# CS522 Project
20144487 이근홍

##필요 라이브러리
~~~~~~~~~~~~~{.sh}
bison
flex
g++ -std=c++11
~~~~~~~~~~~~~
Dot visualization software (for visualizing AST).

##빌드 방법
~~~~~~~~~~~~~{.sh}
make depend
make all
~~~~~~~~~~~~~

#Project1: RE를 읽어서 AST(Abstract Syntax Tree)를 generate 하는 프로그램을 쓰시오.

##Vocabulary set
1. 알파뱃 대소문자
2. 숫자

##연산자
1. Concatenation
이어진 두 regular expression 은 concatenation으로 취급된다.

2. Union
| 기호로 이어진 regular expression들은 union으로 취급된다.

3. Closure
*: 0~무한대까지 반복
+: 1~무한대까지 반복
?: 0번, 혹은 1번 반복

4. Character set
"[asdf]": []안에 있는 문자들을 accept. (a|s|d|f) 와 동등하다.
"[^asdf]": asdf를 제외한 나머지 vocabulary를 accept.
.: 모든 vocabulary를 accept

5. Group
()로 감싸진 부분은 하나의 regular primary로 사용된다.

5. 특수문자
[, ], (, ) 들은 특수문자이다.
이들을 vocabulary로서 사용하기 위해서는 \로 escaping해야 한다.
\[ 이런식으로. \자체를 vocabulary로 사용하기 위해서는 \\로 escaping해야 한다.
""는 epsilon을 나타낸다.

##사용법
build안에 regexp 바이너리를 실행한다.
regular expression을 입력하고 엔터를 누르면 Dot 형식으로 (http://en.wikipedia.org/wiki/DOT_%28graph_description_language%29)
AST를 출력해 준다.
이 형식은 Tulip 등의 프로그램을 통하여 그림으로 만들 수 있다.
