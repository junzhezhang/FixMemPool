# FixMemPool
fixed-block size memory pool, doubly linking with index in an array of object.<br />
array (contigious in memory) is declared before pool initiliazation,<br />
hence array is store out side the Pool.<br />
no BLOCK object needed, but drawback shown when returning pointers to array of block.<br />
no reset Pool, outPool allocation for larger item omitted,<br />
no derivative considered,<br />
all codes  are within this single file, brief test at the end.<br />
//TODO: to debug on listNode scope and index-pointer conversion related issues.<br />
