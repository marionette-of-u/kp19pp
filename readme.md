# 概要
LALR(1) パーサジェネレータです。  
継承元プロジェクト caper（<http://caper.googlecode.com/svn/trunk/caper/site/caper.html>）
<b><i></i></b>

# 入力

## フォーマット
&lt; <b><i>token-header</i></b> &gt; <b><i>token-prefix</i></b> {  
&nbsp;&nbsp;&nbsp;&nbsp;&lt; <b><i>link-direction</i></b> &gt;{  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; , ..., <b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; ;  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;...  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; , ..., <b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; ;  
&nbsp;&nbsp;&nbsp;&nbsp;}  
&nbsp;&nbsp;&nbsp;&nbsp;<b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; , ..., <b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; ;  
&nbsp;&nbsp;&nbsp;&nbsp;...  
&nbsp;&nbsp;&nbsp;&nbsp;<b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; , ..., <b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; ;  
}  

&lt; <b><i>grammar-header</i></b> &gt; <b><i>namespace</i></b> {  
&nbsp;&nbsp;&nbsp;&nbsp;<b><i>nonterminal-symbol</i></b> &lt; <b><i>nonterminal-symbol-type</i></b> &gt;  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: [ <b><i>semantic-action</i></b> ] <b><i>symbol</i></b> ( <b><i>arg-num</i></b> ) , ..., <b><i>symbol</i></b> ( <b><i>arg-num</i></b> )   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| [ <b><i>semantic-action</i></b> ] <b><i>symbol</i></b> ( <b><i>arg-num</i></b> ) , ..., <b>
<i>symbol</i></b> ( <b><i>arg-num</i></b> )   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;...  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| [ <b><i>semantic-action</i></b> ] <b><i>symbol</i></b> ( <b><i>arg-num</i></b> ) , ..., <b>
<i>symbol</i></b> ( <b><i>arg-num</i></b> )   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;;  
}  

##終端記号定義部
カンマで記号を区切る事により、同じ優先順位の終端記号を定義する。  
セミコロンで列を区切る事により、次の優先順位の終端記号を定義する。  
&lt; <b><i>terminal-symbol-type</i></b> &gt;は必須ではない。  

- token-header  
"token_desc"もしくは"token_asc"。前者の場合、終端記号定義部での終端記号優先順位は降順に、後者の場合は昇順になる。  
 
- token-prefix  
アルファベットもしくはアンダースコアから始まり、0文字以上のアルファベットもしくはアンダースコア、数字が続く文字列。  
トークンのenumの型名及びトークンのprefixとなる。省略すると自動的に"token"に設定される。

- link-direction  
"left"もしくは"right"。終端記号の結合方向を定める。

- terminal-symbol  
アルファベットもしくはアンダースコアから始まり、0文字以上のアルファベットもしくはアンダースコア、数字が続く文字列。  
終端記号。

- &lt; terminal-symbol-type &gt;  
終端記号の型。構文定義部で引数を取る終端記号は必ず型がなければならない。