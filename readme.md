# 概要
LALR(1) パーサジェネレータです。  
継承元プロジェクト caper（<http://caper.googlecode.com/svn/trunk/caper/site/caper.html>）
<b><i></i></b>

# 入力

## フォーマット
&lt; <b><i>token-header</i></b> &gt; <b><i>token-prefix</i></b> {  
&nbsp;&nbsp;&nbsp;&nbsp;&lt; <b><i>link-direction</i></b> &gt;{  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; , ..., <b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; ;  
&nbsp;&nbsp;&nbsp;&nbsp;}  
&nbsp;&nbsp;&nbsp;&nbsp;<b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; , ..., <b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; ;  
}  

&lt; <b><i>grammar-header</i></b> &gt; <b><i>namespace</i></b> {  
&nbsp;&nbsp;&nbsp;&nbsp;<b><i>nonterminal-symbol</i></b> &lt; <b><i>nonterminal-symbol-type</i></b> &gt;  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: [ <b><i>semantic-action</i></b> ] <b><i>symbol</i></b> ( <b><i>arg-num</i></b> ) , ..., <b><i>symbol</i></b> ( <b><i>arg-num</i></b> )   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| [ <b><i>semantic-action</i></b> ] <b><i>symbol</i></b> ( <b><i>arg-num</i></b> ) , ..., <b>
<i>symbol</i></b> ( <b><i>arg-num</i></b> )   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;;  
}  

## 終端記号定義部
カンマで記号を区切る事により、同じ優先順位の終端記号を定義する。  
セミコロンで列を区切る事により、次の優先順位の終端記号の列を定義する。  
&lt; <b><i>terminal-symbol-type</i></b> &gt;は必須ではない。  
中括弧で囲まれた結合方向が明示的な終端記号の列や、結合方向が明確でない終端記号の列はどんな順序で何度でも現れて構わない。

- token-header  
"token_desc"もしくは"token_asc"。前者の場合、終端記号定義部での終端記号優先順位は降順に、後者の場合は昇順になる。  
 
- token-prefix  
トークンのenumの型名及びトークンのprefixとなる。省略すると自動的に"token"に設定される。

- link-direction  
"left"もしくは"right"。終端記号の結合方向を定める。

- terminal-symbol  
終端記号。

- &lt; terminal-symbol-type &gt;  
終端記号の型。構文定義部で引数を取る終端記号は必ず型がなければならない。

## 文法定義部
ここでは非終端記号を定義する事によって文法を定める。  
非終端記号には必ず型がなければならない。  
最初に現れた非終端記号が文法全体のrootとなる。  

- &lt; grammar-header &gt;  
"grammar"のみ。将来的に拡張される可能性がある。  

- namespace  
出力されるパーサーを含む名前空間。  

- nonterminal-symbol  
非終端記号。  

- &lt; nonterminal-symbol-type &gt;  
直前の非終端記号の型名となる。  

- [ semantic-action ]  
規則が受理された時に動作するセマンティックアクション。  

- symbol  
終端記号あるいは非終端記号。

- ( arg-num )  
セマンティックアクションの何番目の引数となるかを表す数値。  
省略した場合はその記号は引数として扱われない。  

## 具体的な例

    <token_desc> token{
        <right>{ unary_minus; }
        <left>{
            ast;
            pls, mns;
        }
        l_paren, r_paren;
        id<int>;
    }

    <grammar> grammar{
        E<int>
            : [make_pls] E(0) pls E(1)
            | [make_mns] E(0) mns E(1)
            | [make_mlt] E(0) ast E(1)
            | [make_exp] l_paren E(0) r_paren
            | [make_ums] <unary_minus> mns E(0)
            | [make_id]  id(0)
            ;
    }

# 出力
参照 <http://caper.googlecode.com/svn/trunk/caper/site/caper.html>
ただしtoken_eofは本プログラムではtoken_0に置き換えられている。