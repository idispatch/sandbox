set nocompatible

" --Pathogen
" call pathogen#infect()
" call pathogen#helptags()
 
set statusline=%<\ %n:%f\ %m%r%y%=%-35.(line:\ %l\ of\ %L,\ col:\ %c%V\ (%P)%)
"filetype plugin indent on
 
syntax on
set number
" set mouse=a
set mousehide

set hlsearch
set showmatch
set incsearch
set ignorecase
set autoindent
set history=1000
" set cursorline
" if has("unnamedplus")
"  set clipboard=unnamedplus
"elseif has("clipboard")
"  set clipboard=unnamed
"endif

" set expandtab
set shiftwidth=4
set tabstop=4
set softtabstop=4

" Nerdtree
"autocmd VimEnter * NERDTree
"autocmd VimEnter * wincmd p
"let NERDTreeShowBookmarks=1
"let NERDTreeChDirMode=0
"let NERDTreeQuitOnOpen=0
"let NERDTreeMouseMode=2
"let NERDTreeShowHidden=1
"let NERDTreeIgnore=['\.pyc','\~$','\.swo$','\.swp$','\.git','\.hg','\.svn','\.bzr']
"let NERDTreeKeepTreeInNewTab=1
"let g:nerdtree_tabs_open_on_gui_startup=0
 
source ~/Documents/vividchalk.vim
set background=dark
"colorscheme wombat
