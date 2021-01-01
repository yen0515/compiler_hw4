# hw4 report

|||
|-:|:-|
|Name|黃彥慈|
|ID|0716088|

## How much time did you spend on this project

70 hours.

## Project overview

可分為幾個部分，分別是SymbolTable.hpp與.cpp、SemanticAnalyzer.hpp與.cppc和最後的//&D實作，而各type node的.cpp與.hpp改動不多，多為將private調整為public或是設定一function來return所需要的變數或是資訊，也因次不贅述。

### SymbolTable.hpp與SymbolTable.cpp

我使用了3個class與3個enum來儲存symbol table中應該要有的資訊。  

#### enum

1. enumTypeSet與enumType與上次作業的功能相同，皆是為了判斷該變數的type而設立，也就是symbol table中的'Type'這一項，而功能與上次相同便因此不多贅述。
2. 第三個enum也就是enumKind負責判斷該變數為program、function等哪一種kind，負責判斷table中'Kind'這一項。

#### Attribute class

1. 這個class負責判斷table中'Attribute'這一項，而因為attribute主要負責顯示該變數的值，也因此在這個class中有int、double、string與boolean四種不同type的變數來分別儲存對應type的值。
2. arr_val這個vector則負責存該變數的array值，如i[1][2]的話便會儲存1跟2，便可用此來判斷該變數是否為一個array。
3. 當該變數唯一function kind時，attribute必須儲存該function的return type，而func_arg這個string是便是負責處理這種情況下的attribute需為何。
4. enumTypeSet與enumType便是負責判斷其型別的兩個enum。

#### SymbolEntry class

1. 有兩種constructor，一般正常的便是由一string、enumKind、enumTypeSet、enumType與Attribute組成，而若input為一array則input中的enumType將會變為是一string，直接用string來儲存array的資訊，而使用sym_which來判斷是哪一種情形，並可以使用get_which()來得到sym_which的值。
2. 可以使用get_symkind()來將enumKind轉換為相對應的string。
3. 可以使用get_symtype()來將enumTypeSet與enumType轉換為相對應的string，get_symtype_str()則可以得到array情況下的string。
4. get_shorttype()則不管其為一般情形或是array情形都只return最基本的情形，如integer會return integer，integer[1][2]則也會return integer。
5. get_arr()則透過get_symtype_str()與get_symtype()得到的string來放進split這個不在class內的function，split可以根據輸入的string與要根據哪些char進行分割兩個變數來將一個string分割成一string type vector，如以integer[1][2]作為input，" "作為分割的symbol，則會得到一個vector為("integer","[1][2]"，也因此可以透過兩次的分割，也就是"integer[1][2]"->("integer","[1][2]")->("integer","1","2")，也可以藉此得到array中的scale，get_arr()便可以藉此得到array中的scale，如integer[1][2]會變為一vector("1","2")。
6. get_arrsize()便是透過剛剛提到的get_arr()來得到其array為多少dimension。

#### SymbleTable class

1. onstructor為賦予該table為多少level。
2. addSymbol()可以將該entry插進現在的table中，而table中儲存entry的方式是由一string vector來儲存symbol name，而又可以用該symbol name map至完整的entry，會這樣儲存的原因是因為這樣較好檢查該variable name是否已經被宣告過。
3. isexist()便是根據input的entry name來判斷是否已經有儲存過那個名稱的變數。
4. print()便是負責根據format來print出table，print_format()這個不在class的function可以幫忙print出=與-這兩個table邊框。
5. entry_num()與_add_loop()這兩個function在之後便沒有使用了因此略過。

### SemanticAnalyzer.cpp與SemaanticAnalyzer.hpp

#### 自定義的function

1. can_print()如下點所說。
2. pushScope()可以將現在的環境改到即將新增的table，實作方法則是設定新table的prev為現在的table，並將現在的table更新為新table，最後則是將table push進儲存table的stack中。
3. 相反的，popScope()則將table從stack中pop出，並且將現在的table更新為現在table的prev。
4. print_table()則是如下點所說，若偵測到該boolean為true時觸發該table的print()。
5. func_index()則是當我宣告一function時會將該function的名稱存到一個叫func_var的stack中，而這個function便是檢查該string是否為已經宣告過的function名稱，若有的話則根據其在stack中的位置判斷是第幾個宣告的函數；modify_funcvar()則是相同方法，不過是return一是否為已經宣告過的boolean。
6. modify_loopvar()則與上述相似，不過是判斷是否有被宣告為一loop variable，若有的話則return true，modify_const()則是判斷是否有被宣告為一const。
7. check_exist()則是尋找所有現存的table中是否有宣告過這個變數，為上述提到過的isexist()的完善版，因為發現isexist()在判斷一些狀況時會有錯誤因此多使用了這個function來補足不足。
8. get_shorttype()則是根據上述原理找到該entry後觸發其get_shorttype()；get_arrsize()則是相同道理，不過是觸發其get_arrsize()。
9. error1()與error2()是負責print出各error皆相同的error messege，如<Error>Found in line...等等。

#### 各type node之visit()
1. program node便是新增一table後將program name與其性質constuct成一個entry後加進該table中，之後便進行visitChildNodes()，traverse之後將func_var也就是儲存function name的stack清空，之後print出table並popScope()。
2. decl node則是單純的visitChildNodes()。
3. variable node則較麻煩，首先是要先判斷該variabble的enumType，判斷出其型別之後判斷是否為有賦值得variable；若有的話可以判斷其kind為CONST_KIND，type則為INT_TYPE，並根據這些new一個SymbolEntry，之後檢查該name是否有被宣告過，若有則error，沒有的話則判斷其是否為一loop variable，是的話為error，不是的話則判定為合法，將該entry add進table中；若該variable沒賦值則先判斷其是否為一parameter，是的話則可以知道是PARA_KIND，type為INT_TYPE，根據這些new出一個entry之後檢查是否已經被宣告過與是否為loop variable，都不是的話則合法，將其add進table中；若不是parameter的話則檢查是否為在loop被宣告，是的話則可以知道其kind為LOOP_VAR_KIND，並且一樣判斷是否被宣告過、是否為loop variable，都不是的話add進table中；若既不是parameter也不是loop variable，則變是普通的VAR_KIND，判斷是否被宣告過、是否為loop variable，都不是的話add進table中。而real、string、boolean type皆相同，唯一的不同便是這些type不可能可以成為loop variable。而最後便是arraay type，先判斷其是否為parameter之後new一entry，或已經宣告過則error，之後檢查是否為合格的合法的array，也就是其array index是否都有大於0，而由於array type不可能被宣告為loop variable因此不用檢查，最後一樣是判斷是否宣告過與是否合法，若一切皆正確則add進table。
4. constant node則是先以for_val這個vector來存取for loop的兩個數值，若在該node在for loop中且尚未進到compoundstatement則代表其一定是那兩個數值，便可以成功存取；再來便是將constant node的PType push進expr_stack中以方便後面expression的地方存取其type。
5. function node的話則一樣先根據其enumType來判斷其型別，用一個func_type的string變數來存取現在function的return type為何，之後便根據該function argument的數量才將資訊存進attribute中之後new一個新的entry，之後便是檢查該function名稱是否已經被宣告過，若有的話則error，否則便將其add進table中；而real、string、boolean與void皆相同；最後則是將剛剛收集到的function parameter的type與名稱塞進一vector之後在存進一二維vector中，這也是為什麼需要自訂一func_index()，這樣才會知道在這個二維vector中對應到哪一個function的資訊，在清空一維vector以存取下一個function資訊後print table並pop scope。
6. compoundstatement node則是一function與for loop宣告的結束，也因此將判斷是否在function與判斷是否在for loop宣告的boolean設為false，之後便是new一table之後進行traverse。
7. print便是traverse之後從expr_stack中pop出資訊以檢查是否合法，檢查要print的資訊是否為array或是一void type的function invocation，若是的話則為error，並將一代表error的PTypepush回stack中，否則便是將原來的expression直接push回stack中。
8. binary operator node中先判斷是何種operator，從expr_stack中pop出兩個變數分別就是符號左邊的type與右邊的type，判斷兩邊的type是否符合條件，如'+'便是兩邊為real或是integer，或是兩邊皆為integer則為合法，'>'兩邊是否皆為integer或real等等，若符合條件則將相對應type的PType push回stack中，例如'>'便是push boolean，real+integer便是push real等，若有錯誤的顯示invalid。
9. unary perator node與binary相似，不過這次只從stack中pop出一PType，並檢查其是否符合規則。
10. function invocation則是首先先將argument存進func_inv_para這個vector中，之後則是先檢查該func_inv的名稱是否存在與是否為function的名稱，若不是的話則顯示出相對應的error messege，是的話則從從5.處提及的二維vector中提出相對應function的parameter，之後檢查數量是否相同，相同的話則一一檢查型別是否對應正確，若發生任何錯誤則顯示error，否則便根據該function個return type將相對應的型別PType push回stack中。
11. variable reference node中首先要根據從table中查找出來這個名字的變數其形態為何，並且根據型態來push相對應的PType進入到expr_stack中，並且根據該name來找到當初宣告的type為何，也就是其array的dimension為何，而找到當初宣告的dimension size之後便可以跟現在呼叫做比對，若兩者size相同代表即為no array type，也就是若宣告時為integer[1][2]，而呼叫時亦為integer[1][2]，則代表呼叫時所代表的type即為integer，而若呼叫的size大於定義的size則為error，若小於的話則須計算兩者差多少，並進而得到現在所代表的type，如宣告為integer[1][2]，呼叫時為integer[1]，則可以知道呼叫時所代表的type即為integer[2]。最後則是若之前發生過error則不理會，若未宣告過則error，宣告時為function name則error，array的index不為integer則error，最後則是呼叫的array size大於宣告時的則error。
12. assignment node則是將assign左右的type從expr_stack中pop出來並進行判斷，若左方的dimension不為0則代表其為array type則error，若左方為一常數則error，若右方為一array為error，若loop variable在loop中被修改則error，若左右型別不同則error。
13. read node的實作方法相似，便是從expr_stack中pop出Ptype，並對其進行檢查，若read的對象為array則error，為loop variable則error，為常數則error。
14. if node也是一樣的道理，從expr_stack中pop出PType，檢查若其不為boolean則error。
15. while node與if相同，若不為boolean則error。
16. for node是根據4.中得到的值判斷是否前項大於後項，若是的話則error。
17. return node便一樣pop出值，並根據5.中得到的return type來進行檢查，若return type為空，代表為void type則為error，若與pop出值的型別不相同則error。

### //&D實作
1. 在scanner.l中宣告一int32_t type的變數opt_table，並且在底下的RE中進行定義
2. 在parser.y中將opt_table extern並且在最底下的main中，若opt_table = 1則觸發semantic analyzer中的can_print()，將boolean can_print_table設為true，若該boolean為true才會成功print出table。

## What is the hardest you think in this project

要利用各個boolean與register來判斷出何種error的發生，尤其是要准許特定情形卻阻止大多數的狀況。

## Feedback to T.A.s

建議可將function invocation的測資放到operator之前，因為在operator內即須用到function invocation
