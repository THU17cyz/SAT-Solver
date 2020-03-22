import os

def queen_cnf_generator(n, ext=".dimacs", root_dir='n-queens'):
    if not os.path.exists(root_dir):
        os.makedirs(root_dir)
    lines = []
    lines.append('c '+str(n)+' queens'+'\n')
    diagonal_count = 0
    for i in range(1, n):
        diagonal_count += (i-1)*i//2
    for i in range(1, n+1):
        diagonal_count += (i-1)*i//2
    lines.append('p cnf '+str(n*n)+' ' +
                 str(2*n*(1+(n-1)*n//2)+2*diagonal_count)+'\n')
    # rows
    for i in range(1, n+1):
        nums = []
        for j in range(i*n-n+1, i*n+1):
            nums.append(str(j))
        nums.append('0')
        lines.append(' '.join(nums)+'\n')
        for j in range(-i*n, -i*n+n-1):
            for k in range(j+1, -i*n+n):
                nums = []
                nums.append(str(j))
                nums.append(str(k))
                nums.append('0')
                lines.append(' '.join(nums)+'\n')
        
    # columns
    for i in range(1, n+1):
        nums = []
        for j in range(i, n*n+i, n):
            nums.append(str(j))
        nums.append('0')
        lines.append(' '.join(nums)+'\n')
        for j in range(-i, -i-n*n+n, -n):
            for k in range(j-n, -i-n*n, -n):
                nums = []
                nums.append(str(j))
                nums.append(str(k))
                nums.append('0')
                lines.append(' '.join(nums)+'\n')

    # diagonals
    diagonals = []
    for i in range(2, n+2):
        diagonal = []
        for j in range(1, i):
            diagonal.append((j-1)*n+(i-j))
        diagonals.append(diagonal)
    for i in range(n+2, 2*n+1):
        diagonal = []
        for j in range(n, i-n-1, -1):
            diagonal.append((j-1)*n+(i-j))
        diagonals.append(diagonal)
    for i in range(1, n+1):
        diagonal = []
        for j in range(0, n-i+1):
            diagonal.append((i+j-1)*n+(1+j))
        diagonals.append(diagonal)
    for i in range(2, n+1):
        diagonal = []
        for j in range(1, n-i+2):
            diagonal.append((j-1)*n+(i+j-1))
        diagonals.append(diagonal)
    for diagonal in diagonals:
        # nums = []
        # for num in diagonal:
        #     nums.append(str(num))
        # nums.append('0')
        # lines.append(' '.join(nums))
        l = len(diagonal)
        for i in range(l):
            for j in range(i+1, l):
                nums = []
                nums.append(str(-diagonal[i]))
                nums.append(str(-diagonal[j]))
                nums.append('0')
                lines.append(' '.join(nums)+'\n')
    #print(lines)
    with open(os.path.join(root_dir,str(n)+'queens'+ext), 'w') as f:
        f.writelines(lines)
    

def main(n = 20):
    for i in range(2, n+1):
        queen_cnf_generator(i)

if __name__ == '__main__':
    main(30)
    # queen_cnf_generator(3)
