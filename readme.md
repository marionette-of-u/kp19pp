# 概要
LALR(1) パーサジェネレータです。  
継承元プロジェクト caper（<http://caper.googlecode.com/svn/trunk/caper/site/caper.html>）

# 書式

## 終端記号定義部

    <token_desc> token{
        <right>{ unary_minus; }

        <left>{
            ast;
            pls, mns;
        }

        l_paren, r_paren;
        id<int>;
    }
一行ずつ見ていきましょう。  
`<token_desc> token`  
ここでは「終端記号のトークンの宣言を、優先順位に関して降順に（`<token_desc>`）」「トークンのenum及びprefixを`token`」に指定しています。  
他の設定の余地として、`<token_asc>`（優先順位を昇順）、token部を`hogehoge`や`piyopiyo`などの好きな文字列にできます。  
`<right>{ unary_minus; }`  
ここでは、中括弧内部に現れる終端記号の結合方向を右に設定しています。また、今回の`unary_minus`というトークンは、それ自体が直接構文の中で使われる記号でなくても構いません。実際、この`unary_minus`は構文定義内で優先順位を変更するためだけに使用されます。
`<left>{`  
これも`<right>`と同様、中括弧内に現れる終端記号を左結合に指定するために記述しています。  
`ast;`  
`pls, mns;`  
この三つの終端記号にはそれぞれ、行末にセミコロンが記述されています。終端記号をセミコロンで区切ると、優先順位を変更する事ができます。  
今回では`<token_desc>`と降順になｔっているので優先順位は`ast > pls = mns`になります。  
`l_paren, r_paren;`はそれぞれ、左丸括弧、右丸括弧です。  
`id<int>;`は`int`型の値を持つ終端記号です。終端記号が値を持っていると、セマンティックアクションに引数として使えます。

## 文法定義部

    <grammar> grammar{
        E<int>
            : [make_pls] E(0) pls E(1)
            | [make_mns] E(0) mns E(1)
            | [make_mlt] E(1) ast E(0)
            | [make_exp] l_paren E(0) r_paren
            | [make_ums] <uminus> mns E(0)
            | [make_id]  id(0)
            ;
    }
`<grammar> grammar`において、山括弧で囲まれた部分は「文法を定義する」というヘッダの記述であり必須です。  
その次の`grammar`が、出力された時にパーサーを囲う名前空間となります。  
`E<int>`ここで非終端記号Eの定義を行います。非終端記号には値を表す型が必須になっています。ここではint型を想定しているので`<int>`となっています。  
`[make_pls] E(0) pls E(1)`ブラケットで囲まれた`make_pls`はセマンティックアクションです。ない場合、即ち`[]`だと「セマンティックアクション無し」の文法定義になります。  
そして次の文法定義に現れる`E(0) pls E(1)`は「終端記号`pls`を挟んだ左側の`E`を1番目の引数にと右側の`E`を2番目の引数に取る」という事を意味します。