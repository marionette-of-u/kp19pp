# �T�v
LALR(1) �p�[�T�W�F�l���[�^.   
�p�����v���W�F�N�g�ucaper�v�i<http://caper.googlecode.com/svn/trunk/caper/site/caper.html>�j  
�{�v���O������caper�Ɏ኱�̍������i�Q�ߓI�ɂ͓����j��, �I�[�L���̗D�揇�ʂ�t�^���鎖�ɂ��B��������y�ь���I�ȏՓˉ���̉��ǂ����������̂ł���.   

# ����m�F
<table><tr><td>Compiler</td><td><s>Clang 3.2</s>, <s>MSVC 10.0</s>, MSVC 11.0</td></tr><tr><td>Lib</td><td>Boost 1.53.0</td></tr></table>

# �g����
## ���s�t�@�C���̐���
�e.cpp�t�@�C������L�̃R���p�C���܂��͏�L�ƌ݊����̂���R���p�C���ŃR���p�C��. 

## parser�̐���

    kp19pp [ options ] ifile ofile
C++�̃R�[�h����������ꍇ��`-c++`,   
Vim script�̃R�[�h����������ꍇ��`-vimscript`,   
���O���o�͂���ꍇ��`-log`,   
�������Ԃ��o�͂���ꍇ��`-time`,   
�S�Ă̏������Ԃ��o�͂���ꍇ��`-alltime`��`[ options ]`�ɒǉ�. 

# ����
## �t�H�[�}�b�g
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
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| [ <b><i>semantic-action</i></b> ] &lt; <b><i>precedence-symbol</i></b> &gt; <b><i>symbol</i></b> ( <b><i>arg-num</i></b> ) , ..., <b><i>symbol</i></b> ( <b><i>arg-num</i></b> )   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;;  
}  

## �I�[�L����`��
�J���}�ŋL������؂鎖�ɂ��, �����D�揇�ʂ̏I�[�L�����`����.   
�Z�~�R�����ŗ����؂鎖�ɂ��, ���̗D�揇�ʂ̏I�[�L���̗���`����.   
&lt; terminal-symbol-type &gt;�͕K�{�ł͂Ȃ�.   
�I�[�L���͕��@��`���ɃV�[�P���X�Ƃ��ċL�q����Ȃ��Ă�, �P�ɋK���̗D�揇�ʂ�ύX���邽�߂����ɑ��݂��Ă��ǂ�  
�i��q�F���@��`��, &lt; precedence-symbol &gt;�j.   

- token-header  
"token_desc"��������"token_asc". �O�҂̏ꍇ, �I�[�L����`���ł̏I�[�L���D�揇�ʂ͍~����, ��҂̏ꍇ�͏����ɂȂ�.   
 
- token-prefix  
�g�[�N����enum�̌^���y�уg�[�N����prefix�ƂȂ�. �ȗ�����Ǝ����I��"token"�ɐݒ肳���. 

- link-direction  
"left"��������"right"��������"nonassoc". �I�[�L���̌����������߂�. 

- terminal-symbol  
�I�[�L��. 

- &lt; terminal-symbol-type &gt;  
�I�[�L���̌^. ���@��`���ň��������I�[�L���͕K���^���Ȃ���΂Ȃ�Ȃ�. 

## ���@��`��
�����ł͔�I�[�L�����`���鎖�ɂ���ĕ��@���߂�.   
��I�[�L���ɂ͕K���^���Ȃ���΂Ȃ�Ȃ�.   
�ŏ��Ɍ��ꂽ��I�[�L�������@�S�̂�root�ƂȂ�.   

- &lt; grammar-header &gt;  
"grammar"�̂�. �����I�Ɋg�������\��������.   

- namespace  
�o�͂����p�[�T�[���܂ޖ��O���.   

- nonterminal-symbol  
��I�[�L��.   

- &lt; nonterminal-symbol-type &gt;  
���O�̔�I�[�L���̌^���ƂȂ�.   

- [ semantic-action ]  
�K�����󗝂��ꂽ���ɓ��삷��Z�}���e�B�b�N�A�N�V����.   

- symbol  
�I�[�L�����邢�͔�I�[�L��. 

- ( arg-num )  
�Z�}���e�B�b�N�A�N�V�����̉��Ԗڂ̈����ƂȂ邩��\�����l.   
�ȗ������ꍇ�͂��̋L���͈����Ƃ��Ĉ����Ȃ�.   

- &lt; precedence-symbol &gt;  
�I�[�L��precedence-symbol���^����ꂽ�K���͋����I�ɂ��̏I�[�L���̗D�揇�ʂɕύX�����.   

## ��̓I�ȗ�

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

�ȏ��,  
<pre><code>�u�D�揇�ʂ�1��, 2��, 3��, ...�Ɛ����n�߁v, �u��A�̃g�[�N���̌^��token�^�Ƃ��v, 
�u�D�揇��1��, �E�����̃g�[�N��unary_minus���`�v. 
�u�D�揇��2��, �������̃g�[�N��ast���`�v. 
�u�D�揇��3��, �������̃g�[�N��pls��mns���`�v. 
�u�D�揇��4��, ���������w��Ȃ��̃g�[�N��l_paren, r_paren���`�v. 
�u�D�揇��5��, ���������w��Ȃ���int�^�̒l��ێ�����g�[�N��id���`�v. 
�i���@��`���̐�����caper, yacc�ȂǂƂقƂ�Ǔ����Ȃ��ߏȗ��j
</code></pre>
�Ƃ����Ӗ��̕��@���`������ł���. 

# �o��
�Q�� <http://caper.googlecode.com/svn/trunk/caper/site/caper.html>
������token\_eof�͖{�v���O�����ł�token\_0�ɒu���������Ă���. 

# ���C�Z���X
�����|�W�g����LICENSES�Q��. 