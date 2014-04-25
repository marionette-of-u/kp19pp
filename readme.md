# 概要
LALR(1) パーサジェネレータ.   
継承元プロジェクト「caper」（<http://caper.googlecode.com/svn/trunk/caper/site/caper.html>）  
本プログラムはcaperに若干の高速化（漸近的には同等）と, 終端記号の優先順位を付与する事による曖昧さ回避及び限定的な衝突回避の改良を加えたものである.   

# 動作確認
<table><tr><td>Compiler</td><td>MSVC 12.0</td></tr><tr><td>Lib</td><td>Boost 1.53.0</td></tr></table>

# 使い方
## 実行ファイルの生成
各.cppファイルを上記のコンパイラまたは上記と互換性のあるコンパイラでコンパイル. 

## parserの生成

    kp19pp [ options ] ifile ofile
C++のコード生成をする場合は`-c++`,   
Haskellのコード生成をする場合は`-haskell`,   
Vim scriptのコード生成をする場合は`-vimscript`,   
ログを出力する場合は`-log`,   
処理時間を出力する場合は`-time`を`[ options ]`に追加. 

# 入力
## フォーマット
&lt; <b><i>token-header</i></b> &gt; <b><i>token-prefix</i></b> {  
&nbsp;&nbsp;&nbsp;&nbsp;&lt; <b><i>link-direction</i></b> &gt;{  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; , ..., <b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; ;  
&nbsp;&nbsp;&nbsp;&nbsp;}  
&nbsp;&nbsp;&nbsp;&nbsp;<b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; , ..., <b><i>terminal-symbol</i></b> &lt; <b><i>terminal-symbol-type</i></b> &gt; ;  
}  

&lt; <b><i>grammar-header</i></b> &gt; <b><i>namespace</i></b> {  
&nbsp;&nbsp;&nbsp;&nbsp;[ <b><i>default-semantic-action</b></i> ]  
&nbsp;&nbsp;&nbsp;&nbsp;<b><i>nonterminal-symbol</i></b> &lt; <b><i>nonterminal-symbol-type</i></b> &gt;  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| [ <b><i>semantic-action</i></b> ] <b><i>symbol</i></b> &lt; <b><i>arg-num</i></b> &gt; , ..., <b> <i>symbol</i></b> &lt; <b><i>arg-num</i></b> &gt;   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| [ <b><i>semantic-action</i></b> ] &#40; <b><i>symbol</i></b> &lt; <b><i>arg-num</i></b> &gt; , ..., <b> <i>symbol</i></b> &lt; <b><i>arg-num</i></b> &gt; &#41;   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| [ <b><i>semantic-action</i></b> ] &lt; <b><i>precedence-symbol</i></b> &gt; <b><i>symbol</i></b> &lt; <b><i>arg-num</i></b> &gt; , ..., <b><i>symbol</i></b> &lt; <b><i>arg-num</i></b> &gt;   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|   [ <b><i>semantic-action</i></b> ] <b><i>symbol-or-group</i></b> [ <b><i>semantic-action</i></b> ] &lt; <b><i>type</i></b> &gt; <b><i>quantifier</i></b>  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;;  
}  

## 終端記号定義部
カンマで記号を区切る事により, 同じ優先順位の終端記号を定義する.   
セミコロンで列を区切る事により, 次の優先順位の終端記号の列を定義する.   
&lt; terminal-symbol-type &gt;は必須ではない.   
終端記号は文法定義部にシーケンスとして記述されなくても, 単に規則の優先順位を変更するためだけに存在しても良い  
（後述：文法定義部, &lt; precedence-symbol &gt;）.   

- token-header  
token_descもしくはtoken_asc. 前者の場合, 終端記号定義部での終端記号優先順位は降順に, 後者の場合は昇順になる.   
 
- token-prefix  
トークンのenumの型名及びトークンのprefixとなる. 省略すると自動的に"token"に設定される. 

- link-direction  
leftもしくはrightもしくはnonassoc. 終端記号の結合方向を定める. 

- terminal-symbol  
終端記号. 

- terminal-symbol-type  
終端記号の型. 文法定義部で引数を取る終端記号は必ず型がなければならない. 

## 文法定義部
ここでは非終端記号を定義する事によって文法を定める.   
非終端記号には必ず型がなければならない.   
最初に現れた非終端記号が文法全体のrootとなる.   

- grammar-header  
"lalr"のみ. 将来的に拡張される可能性がある.   

- namespace  
出力されるパーサーを含む名前空間.   

- nonterminal-symbol  
非終端記号.   

- nonterminal-symbol-type  
直前の非終端記号の型名となる.   
必須ではない.  

- semantic-action  
規則が受理された時に動作するセマンティックアクション.   

- symbol  
終端記号あるいは非終端記号. 

- arg-num  
セマンティックアクションの何番目の引数となるかを表す数値.   
省略した場合はその記号は引数として扱われない.   

- precedence-symbol  
終端記号precedence-symbolが与えられた規則は強制的にその終端記号の優先順位に変更される.   

- symbol-or-group  
量化子修飾対象のシンボルまたはパーレンで囲まれたグループ.  

- type  
量化子修飾セマンティックアクションの返す型.  

- quantifier  
量化子修飾演算子.  
 - ?  
 0回または1回にマッチ.  
 - *  
 0回以上にマッチ
 - +  
 一回以上にマッチ

# 出力
参照 <http://caper.googlecode.com/svn/trunk/caper/site/caper.html>
ただしtoken\_eofは本プログラムではtoken\_0に置き換えられている. 

# ライセンス
同リポジトリのLICENSES参照. 
