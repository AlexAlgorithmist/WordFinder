import requests


with open("wordsRU2.txt", "a", encoding="UTF-8") as f:
    for nletters in list(range(100, 0, -1)):
        wordP = ""
        index = "200"
        while index == "200":
            fromwhere = wordP

            req = requests.get(f"https://ru.wiktionary.org/w/index.php?title=%D0%9A%D0%B0%D1%82%D0%B5%D0%B3%D0%BE%D1%80%D0%B8%D1%8F:%D0%A1%D0%BB%D0%BE%D0%B2%D0%B0_%D0%B8%D0%B7_{nletters}_%D0%B1%D1%83%D0%BA%D0%B2/ru&pagefrom={fromwhere}#mw-pages")
            print(f"{str(nletters): <4}", index, fromwhere)
            src: str = req.text
            try:
                index = str(src.split("Показан")[1][2:5])
            except IndexError:
                index = "0"
                continue
            print(f"{str(nletters): <4}", index, fromwhere)
            res = src.split('<div class="mw-category-group">')[1:]
            res[-1] = res[-1].split("</div>")[0]
            for line in res:
                res2 = line.split('</li>')[1:-1]
                for NotAWord in res2:
                    wordP = NotAWord[:-4].split(">")[-1]
                    if len(NotAWord) != nletters:
                        continue
                    available = "qwertyuiopasdfghjkzxcvbnm\n"
                    # available = "йцукенгшщзхъфывапролджэячсмитьбюё\n"
                    for char in wordP:
                        if char not in available:
                            break
                    else:
                        f.write(wordP + "\n")
                f.flush()
