# �T�v
LALR(1) �p�[�T�W�F�l���[�^�ł��B  
�p�����v���W�F�N�g caper�i<http://caper.googlecode.com/svn/trunk/caper/site/caper.html>�j
<b><i></i></b>

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
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;;  
}  

## �I�[�L����`��
�J���}�ŋL������؂鎖�ɂ��A�����D�揇�ʂ̏I�[�L�����`����B  
�Z�~�R�����ŗ����؂鎖�ɂ��A���̗D�揇�ʂ̏I�[�L���̗���`����B  
&lt; <b><i>terminal-symbol-type</i></b> &gt;�͕K�{�ł͂Ȃ��B  
�����ʂň͂܂ꂽ���������������I�ȏI�[�L���̗��A�������������m�łȂ��I�[�L���̗�͂ǂ�ȏ����ŉ��x�ł�����č\��Ȃ��B

- token-header  
"token_desc"��������"token_asc"�B�O�҂̏ꍇ�A�I�[�L����`���ł̏I�[�L���D�揇�ʂ͍~���ɁA��҂̏ꍇ�͏����ɂȂ�B  
 
- token-prefix  
�g�[�N����enum�̌^���y�уg�[�N����prefix�ƂȂ�B�ȗ�����Ǝ����I��"token"�ɐݒ肳���B

- link-direction  
"left"��������"right"�B�I�[�L���̌����������߂�B

- terminal-symbol  
�I�[�L���B

- &lt; terminal-symbol-type &gt;  
�I�[�L���̌^�B�\����`���ň��������I�[�L���͕K���^���Ȃ���΂Ȃ�Ȃ��B

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

# �o��
�Q�� <http://caper.googlecode.com/svn/trunk/caper/site/caper.html>
������token_eof�͖{�v���O�����ł�token_0�ɒu���������Ă���B