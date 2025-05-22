def num2str(num):
    return f"{str(hex(num)).split('x')[1]:0>2}"


def num2str2(num):  # num <= 0xffffffff
    basic = f"{str(hex(num)).split('x')[1]:0>8}"
    return f"\\x{basic[0:2]:0>2}\\x{basic[2:4]:0>2}\\x{basic[4:6]:0>2}\\x{basic[6:8]:0>2}"


def char2numEN(ch):
    match ch:
        case "a":
            return 1
        case "b":
            return 2
        case "c":
            return 3
        case "d":
            return 4
        case "e":
            return 5
        case "f":
            return 6
        case "g":
            return 7
        case "h":
            return 8
        case "i":
            return 9
        case "j":
            return 10
        case "k":
            return 11
        case "l":
            return 12
        case "m":
            return 13
        case "n":
            return 14
        case "o":
            return 15
        case "p":
            return 16
        case "q":
            return 17
        case "r":
            return 18
        case "s":
            return 19
        case "t":
            return 20
        case "u":
            return 21
        case "v":
            return 22
        case "w":
            return 23
        case "x":
            return 24
        case "y":
            return 25
        case "z":
            return 27
        case _:
            return 0


def char2numRU(ch):
    match (ch):
        case "а":
            return 1
        case "б":
            return 2
        case "в":
            return 3
        case "г":
            return 4
        case "д":
            return 5
        case "е":
            return 6
        case "ё":
            return 7
        case "ж":
            return 8
        case "з":
            return 9
        case "и":
            return 10
        case "й":
            return 11
        case "к":
            return 12
        case "л":
            return 13
        case "м":
            return 14
        case "н":
            return 15
        case "о":
            return 16
        case "п":
            return 17
        case "р":
            return 18
        case "с":
            return 19
        case "т":
            return 20
        case "у":
            return 21
        case "ф":
            return 22
        case "х":
            return 23
        case "ц":
            return 24
        case "ч":
            return 25
        case "ш":
            return 27
        case "щ":
            return 28
        case "ъ":
            return 29
        case "ы":
            return 30
        case "ь":
            return 31
        case "э":
            return 32
        case "ю":
            return 33
        case "я":
            return 34
        case _:
            return 0


print(num2str(26), "\x1a")
print(num2str(249), "\xf9")
with open("wordsRU24C.txt", "wb+") as f:
    with open("wordsRU2.txt", "r", encoding="UTF-8") as fBase:
        lastCount = 0
        count = 0
        groupsCount = 100
        wordsPre = {i: [0, i] for i in range(1, groupsCount)}
        thiswords = []
        i = 0
        for s in fBase.readlines():
            s = s[:-1]
            count = len(s)
            wordsPre[count].append(s)
            wordsPre[count][0] += 1
            i += 1
    words = []
    groupsCount = 0
    for group in list(wordsPre.values()):
        if group[0] == 0:
            continue
        groupsCount += 1
        words.append(group)

    words.reverse()
    print(0xffffffff)
    print(groupsCount, f"{num2str2(groupsCount)}")
    i = 0
    exec(f"f.write(b'{num2str2(groupsCount)}')")
    for group in words:
        if group[0] == 0:
            continue
        print(f"{num2str2(group[0])} {num2str2(group[1])}")
        exec(f"f.write(b'{num2str2(group[0])}')")
        exec(f"f.write(b'{num2str2(group[1])}')")
        for word in group[2:]:
            [exec(f"f.write(b'\\x{num2str(char2numRU(a))}')") for a in word]
        i += 1
