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
이 형식은 Tulip, graphviz 등의 프로그램을 통하여 그림으로 만들 수 있다.

#Project2: Regular Expression의 AST를 보고 minimal state DFA 만들기

프로그램을 실행하면 DOT형식의 여러 그래프를 생성한다.
오토마타의 경우에는 cyan 색으로 칠해진 곳이 start state,
두 겹을 된 동그라미는 final state들을 의미한다.
각 edge는 기본적으로 문자를 만났을 때의 rule이다.
문자 집합이 여집합을 나타낼 경우에는 __EXCEPT__로 시작한다.

##오토마타 만들기
~~~~~~~~~~~~~{.cpp}
Automata rough_automata = Automata::createEpsilonAutomata(last_accept_node);
~~~~~~~~~~~~~
Epsilon을 사용하는 오토마타를 만든다.
이 결과는 digraph rough_automata 를 생성한다.

##Ambiguity 제거
~~~~~~~~~~~~~{.cpp}
Automata normal_NFA = Automata::removeAmbiguity(rough_automata);
~~~~~~~~~~~~~
일반적인 오토마타는 ambiguity를 포함할 수 있다 (문자 'a'를 만났을 때 갈 수 있는 state가 2개 이상).
이를 epsilon만을 사용하도록 고쳐준다.
물론 정규 언어로 만든 오토마타에는 이러한 ambiguity가 없다.
이 결과는 digraph normal_NFA를 생성한다.

##Epsilon 제거
~~~~~~~~~~~~~{.cpp}
Automata DFA = Automata::removeEpsilon(normal_NFA, true);
~~~~~~~~~~~~~
NFA에서 Epsilon closure를 취하여 DFA로 만든다.
두 번째 매개변수는 Failure state를 포함할 것인가의 여부를 나타낸다.
이 결과는 digraph DFA를 생성한다.

##Equivalent State 합병
~~~~~~~~~~~~~{.cpp}
Automata mDFA = Automata::minimalize(DFA);
~~~~~~~~~~~~~
DFA에서 equivalent state들을 모두 합병하여 minimal-DFA를 만든다.
이 결과는 digraph mDFA를 생성한다.