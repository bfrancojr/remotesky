import functools
from subprocess import call
import os
import random
import time
import socket
from piui import PiUi

current_dir = os.path.dirname(os.path.abspath(__file__))

class RemoteSkyUi(object):

    def __init__(self):
        self.title = None
        self.txt = None
        self.img = None
        self.ui = PiUi(img_dir=os.path.join(current_dir, 'imgs'))
        self.src = "sunset.png"

    def take_photo(self):
        self.page = self.ui.new_ui_page(title="Take Photo", prev_text="Back", onprevclick=self.main_menu)
        self.title = self.page.add_textbox("No photo", "h1")
        self.img = self.page.add_image("sunset.png")
        plus = self.page.add_button("Take Photo", self.ontakephoto)

    def main_menu(self):
        self.page = self.ui.new_ui_page(title="RemoteSky Control Panel")
        self.list = self.page.add_list()
        self.list.add_item("Take Photo", chevron=True, onclick=self.take_photo)
        self.ui.done()

    def main(self):
        self.main_menu()
        self.ui.done()

    def ontakephoto(self):
        photos = self.capturePhoto()
        self.title.set_text("Photo taken")
        self.img.set_src(photos['thumb'])

    def capturePhoto(self):
        data = {}
        sock = socket.create_connection(('localhost', 5555))
        try:
            data['photo'] = sock.recv(10000)
            data['thumb'] = sock.recv(10000)
        finally:
            sock.close()

        return data


def main():
  ui = RemoteSkyUi()
  ui.main()

if __name__ == '__main__':
    main()
