# �T�v
LALR(1) �p�[�T�W�F�l���[�^�ł��B  
�p�����v���W�F�N�g caper�i<http://caper.googlecode.com/svn/trunk/caper/site/caper.html>�j
<b><i></i></b>

# ����

## �t�H�[�}�b�g
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

##�I�[�L����`��
�J���}�ŋL������؂鎖�ɂ��A�����D�揇�ʂ̏I�[�L�����`����B  
�Z�~�R�����ŗ����؂鎖�ɂ��A���̗D�揇�ʂ̏I�[�L�����`����B  
&lt; <b><i>terminal-symbol-type</i></b> &gt;�͕K�{�ł͂Ȃ��B  

- token-header  
"token_desc"��������"token_asc"�B�O�҂̏ꍇ�A�I�[�L����`���ł̏I�[�L���D�揇�ʂ͍~���ɁA��҂̏ꍇ�͏����ɂȂ�B  
 
- token-prefix  
�A���t�@�x�b�g�������̓A���_�[�X�R�A����n�܂�A0�����ȏ�̃A���t�@�x�b�g�������̓A���_�[�X�R�A�A����������������B  
�g�[�N����enum�̌^���y�уg�[�N����prefix�ƂȂ�B�ȗ�����Ǝ����I��"token"�ɐݒ肳���B

- link-direction  
"left"��������"right"�B�I�[�L���̌����������߂�B

- terminal-symbol  
�A���t�@�x�b�g�������̓A���_�[�X�R�A����n�܂�A0�����ȏ�̃A���t�@�x�b�g�������̓A���_�[�X�R�A�A����������������B  
�I�[�L���B

- &lt; terminal-symbol-type &gt;  
�I�[�L���̌^�B�\����`���ň��������I�[�L���͕K���^���Ȃ���΂Ȃ�Ȃ��B