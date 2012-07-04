# �T�v
LALR(1) �p�[�T�W�F�l���[�^�B  
�p�����v���W�F�N�g�ucaper�v�i<http://caper.googlecode.com/svn/trunk/caper/site/caper.html>�j  
�{�v���O������caper�Ɏ኱�̍������i�Q�ߓI�ɂ͓����j�ƁA�I�[�L���̗D�揇�ʂ�t�^���鎖�ɂ��B��������y�ь���I�ȏՓˉ���̉��ǂ����������̂ł���B  

# ����m�F
<table><tr><td>Compiler</td><td>Clang 3.2, MSVC 10.0</td></tr><tr><td>Lib</td><td>Boost 1.50.0</td></tr></table>

# �g����
## ���s�t�@�C���̐���
�e.cpp�t�@�C������L�̃R���p�C���܂��͏�L�ƌ݊����̂���R���p�C���ŃR���p�C���B

## parser�̐���

    kp19pp [ options ] ifile ofile
���݂̏��AC++�̐����ɂ����Ή����Ă��Ȃ� 

    kp19pp -c++ ifile.txt ofile.hpp
���O���o�͂���ꍇ��`-log`�A  
�������Ԃ��o�͂���ꍇ��`-time`�A  
�S�Ă̏������Ԃ��o�͂���ꍇ��`-alltime`��`[ options ]`�ɒǉ��B

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
�J���}�ŋL������؂鎖�ɂ��A�����D�揇�ʂ̏I�[�L�����`����B  
�Z�~�R�����ŗ����؂鎖�ɂ��A���̗D�揇�ʂ̏I�[�L���̗���`����B  
&lt; terminal-symbol-type &gt;�͕K�{�ł͂Ȃ��B  
�I�[�L���͕��@��`���ɃV�[�P���X�Ƃ��ċL�q����Ȃ��Ă��A�P�ɋK���̗D�揇�ʂ�ύX���邽�߂����ɑ��݂��Ă��ǂ�  
�i��q�F���@��`���A&lt; precedence-symbol &gt;�j�B  

- token-header  
"token_desc"��������"token_asc"�B�O�҂̏ꍇ�A�I�[�L����`���ł̏I�[�L���D�揇�ʂ͍~���ɁA��҂̏ꍇ�͏����ɂȂ�B  
 
- token-prefix  
�g�[�N����enum�̌^���y�уg�[�N����prefix�ƂȂ�B�ȗ�����Ǝ����I��"token"�ɐݒ肳���B

- link-direction  
"left"��������"right"��������"nonassoc"�B�I�[�L���̌����������߂�B

- terminal-symbol  
�I�[�L���B

- &lt; terminal-symbol-type &gt;  
�I�[�L���̌^�B���@��`���ň��������I�[�L���͕K���^���Ȃ���΂Ȃ�Ȃ��B

## ���@��`��
�����ł͔�I�[�L�����`���鎖�ɂ���ĕ��@���߂�B  
��I�[�L���ɂ͕K���^���Ȃ���΂Ȃ�Ȃ��B  
�ŏ��Ɍ��ꂽ��I�[�L�������@�S�̂�root�ƂȂ�B  

- &lt; grammar-header &gt;  
"grammar"�̂݁B�����I�Ɋg�������\��������B  

- namespace  
�o�͂����p�[�T�[���܂ޖ��O��ԁB  

- nonterminal-symbol  
��I�[�L���B  

- &lt; nonterminal-symbol-type &gt;  
���O�̔�I�[�L���̌^���ƂȂ�B  

- [ semantic-action ]  
�K�����󗝂��ꂽ���ɓ��삷��Z�}���e�B�b�N�A�N�V�����B  

- symbol  
�I�[�L�����邢�͔�I�[�L���B

- ( arg-num )  
�Z�}���e�B�b�N�A�N�V�����̉��Ԗڂ̈����ƂȂ邩��\�����l�B  
�ȗ������ꍇ�͂��̋L���͈����Ƃ��Ĉ����Ȃ��B  

- &lt; precedence-symbol &gt;  
�I�[�L��precedence-symbol���^����ꂽ�K���͋����I�ɂ��̏I�[�L���̗D�揇�ʂɕύX�����B  

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

�ȏ�́A 
<pre><code>�u�D�揇�ʂ�1�ʁA2�ʁA3�ʁA...�Ɛ����n�߁v�A�u��A�̃g�[�N���̌^��token�^�Ƃ��v�A
�u�D�揇��1�ʁA�E�����̃g�[�N��unary_minus���`�v�B
�u�D�揇��2�ʁA�������̃g�[�N��ast���`�v�B
�u�D�揇��3�ʁA�������̃g�[�N��pls��mns���`�v�B
�u�D�揇��4�ʁA���������w��Ȃ��̃g�[�N��l_paren, r_paren���`�v�B
�u�D�揇��5�ʁA���������w��Ȃ���int�^�̒l��ێ�����g�[�N��id���`�v�B
�i���@��`���̐�����caper�Ayacc�ȂǂƂقƂ�Ǔ����Ȃ��ߏȗ��j
</code></pre>
�Ƃ����Ӗ��̕��@���`������ł���B

# �o��
�Q�� <http://caper.googlecode.com/svn/trunk/caper/site/caper.html>
������token\_eof�͖{�v���O�����ł�token\_0�ɒu���������Ă���B

# ���C�Z���X
�����|�W�g����LICENSES�Q�ƁB