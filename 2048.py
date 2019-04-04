#!/usr/bin/python
# coding=utf-8

# A GAME CALLED 2048
# BY ZHANG YAXIN

import curses
from random import randrange, choice
from collections import defaultdict

actions = ['Up', 'Left', 'Down', 'Right', 'Restart', 'Exit']
letter_codes = [ord(ch) for ch in 'WASDRQwasdrq']
actions_dict = dict(zip(letter_codes, actions * 2))

def get_user_action(keyboard):
    #获取用户输入
    char = "N"
    while char not in actions_dict:
        char = keyboard.getch()
    return actions_dict[char]

def transpose(field):
    return [list(row) for row in zip(*field)]
def invert(field):
    return [row[::-1] for row in field]

class GameField(object):
    def __init__(self,height=4,width=4,win=2048):
        self.height,self.width = height,width
        self.win_value = win    #赢的目标
        self.score = 0          #目前分数
        self.highest = 0        #最高分
        self.reset()            #重置棋盘

    def reset(self):
        #重置棋盘
        if self.score > self.highest:
            self.highest = self.score
        self.score = 0
        #棋盘项列表
        self.field = [[0 for i in range(self.width)] for j in range(self.height)]
        #随机生成两个2或4
        self.spawn()
        self.spawn()

    def spawn(self):
        #随机生成2或4
        new_element = 4 if randrange(100) > 89 else 2
        (i,j) = choice([(i,j) for i in range(self.width) for j in range(self.height) if self.field[i][j] == 0])
        self.field[i][j] = new_element

    def move(self, direction):
        def move_row_left(row):
            def tighten(row):
                #将行中元素紧凑
                new_row = [i for i in row if i != 0]    #挑出此行非零元素
                new_row += [0 for i in range(len(row) - len(new_row))]  #补零
                return new_row
            def merge(row):
                #合并
                pair = False
                new_row = []
                for i in range(len(row)):
                    if pair:
                        new_row.append(2*row[i])
                        self.score += 2*row[i]  #得分
                        pair = False
                    else:
                        if (i+1) < len(row) and row[i] == row[i+1]:
                            pair = True
                            new_row.append(0)
                        else:
                            new_row.append(row[i])
                assert len(new_row) == len(row),new_row
                return new_row
            return tighten(merge(tighten(row)))

        #这一块不是很懂
        moves = {}
        moves['Left']  = lambda field:                              \
                [move_row_left(row) for row in field]
        moves['Right'] = lambda field:                              \
                invert(moves['Left'](invert(field)))
        moves['Up']    = lambda field:                              \
                transpose(moves['Left'](transpose(field)))
        moves['Down']  = lambda field:                              \
                transpose(moves['Right'](transpose(field)))

        if direction in moves:
            if self.move_possible(direction):
                self.field = moves[direction](self.field)
                self.spawn()
                return True
            else:
                return False

    def move_possible(self, direction):
        #此方向是否可移动
        def row_is_left_movable(row):
            def change(i):
                if row[i] == 0 and row[i + 1] != 0: #可以移动
                    return True
                if row[i] != 0 and row[i + 1] == row[i]: #可以合并
                    return True
                return False
            return any(change(i) for i in range(len(row) - 1))

        check = {}
        check['Left']  = lambda field:                              \
                any(row_is_left_movable(row) for row in field)
        check['Right'] = lambda field:                              \
                 check['Left'](invert(field))
        check['Up']    = lambda field:                              \
                check['Left'](transpose(field))
        check['Down']  = lambda field:                              \
                check['Right'](transpose(field))

        if direction in check:
            return check[direction](self.field)
        else:
            return False


    def is_win(self):
        for row in self.field:
            for i in row:
                if i >= self.win_value:
                    return True
        return False

    def is_gameover(self):
        for direction in actions:
            if self.move_possible(direction):
                return False
        return True

    def draw(self,screen):
        #画图
        welcome = '----WELCOME TO MY GAME 2048----'
        help_str1 = 'Up[W],Down[S],Left[A],Right[D]'
        help_str2 = '     Restart[R],Exit[Q]'
        win_str = '-----------YOU  WIN!-----------'
        over_str = '----------GAME  OVER!----------'

        def cast(string):
            screen.addstr(string + '\n')

        #绘制水平分割线
        def draw_hor_separator():
            line = '+' + ('------+' * self.width)
            separator = defaultdict(lambda: line)
            if not hasattr(draw_hor_separator, "counter"):
                draw_hor_separator.counter = 0
            cast(separator[draw_hor_separator.counter])
            draw_hor_separator.counter += 1

        def draw_row(row):
            cast(''.join('|{: ^5} '.format(num) if num > 0 else '|      ' for num in row) + '|')

        screen.clear()
        cast(welcome)
        cast('SCORE: ' + str(self.score))
        if 0 != self.highest:
            cast('HIGHSCORE: ' + str(self.highest))
        for row in self.field:
            draw_hor_separator()
            draw_row(row)
        draw_hor_separator()
        if self.is_win():
            cast(win_str)
        else:
            if self.is_gameover():
                cast(over_str)
            else:
                cast(help_str1)
        cast(help_str2)



def main(stdscr):

    def init():
        game_field.reset()
        return 'Game'

    def not_game(state):
        #画出 GameOver 或者 Win 的界面
        game_field.draw(stdscr)
        #读取用户输入得到action，判断是重启游戏还是结束游戏
        action = get_user_action(stdscr)

        responses = defaultdict(lambda: state) #默认是当前状态，没有行为就会一直在当前界面循环
        responses['Restart'], responses['Exit'] = 'Init', 'Exit' #对应不同的行为转换到不同的状态
        return responses[action]

    def game():
        #画出当前棋盘状态
        game_field.draw(stdscr)
        #读取用户输入得到action
        action = get_user_action(stdscr)
        if action == 'Restart':
            return 'Init'
        if action == 'Exit':
            return 'Exit'
        if game_field.move(action):
            if game_field.is_win():
                return 'Win'
            elif game_field.is_gameover():
                return 'Gameover'
            else:
                return 'Game'
        print 'move error!'
        return 'Game'


    state_actions = {
            'Init': init,
            'Win': lambda: not_game('Win'),
            'Gameover': lambda: not_game('Gameover'),
            'Game': game
        }
    curses.use_default_colors()
    game_field = GameField(win = 32)
    state = 'Init'

    #状态机开始循环
    while state != 'Exit':
        state = state_actions[state]()

curses.wrapper(main)
