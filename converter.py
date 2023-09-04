def convert(str1):
    list1=str1.split()#removed spaces
    if list1[0]=="802.15.4":
        return
    list2=[]#removes "/" & ">"
    for i in list1:
        if i!="/" and i!=">":
            if i[0]!="(":
                list2.append(i)

    if list2[3][0]=="f" or list2[3][0]==":":
        list2.insert(3,"")
    if list2[4][0]=="f" or list2[4][0]==":":
        list2.insert(4,"")
    if list2[5][0]=="f" or list2[5][0]==":":
        list2.insert(5,"")
    if len(list2)==9:
        list2.insert(-1,"")
    print(list2)
    return list2

