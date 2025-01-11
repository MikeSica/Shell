#!/usr/bin/env python3

from unittest import TestCase

import unittest

import os.path
import sys
import subprocess
import random
import re

from shell_test_helpers import *

TOKENIZE = "./tokenize"
SHELL = "./shell"



class ShellTests(ShellTestCase):
    def __init__(self, *args, **kwargs):
        super().__init__(SHELL, *args, **kwargs)

    def test01(self):
        """Recognizes a simple non-special token"""
        self.assertEqual(sh('echo a | ./tokenize'), 'a')

    def test02(self):
        """Recognizes two non-special tokens"""
        self.assertEqual(sh("echo 'a b' | ./tokenize"), "a\nb")
    
    def test03(self):
        """Recognizes three non-special multi-char tokens"""
        self.assertEqual(
                sh("echo 'foo_bar baz      hello' | ./tokenize"), 
                "foo_bar\nbaz\nhello")


    def test04(self):
        """Recognizes special characters as tokens"""
        self.assertEqual(sh("echo '(;|)<>' | ./tokenize"), "(\n;\n|\n)\n<\n>")

    def test05(self):
        """Recognizes a string"""
        self.assertEqual(sh("echo '\"hello world\"' | ./tokenize"), "hello world")

    def test06(self):
        """Recognizes strings mixed with other tokens"""
        self.assertEqual(
                sh("echo 'foo \"Lorem ipsum dolor sit amet\" < bar \"consectetur (adipiscing; >elit\"' | ./tokenize"), 
                "foo\nLorem ipsum dolor sit amet\n<\nbar\nconsectetur (adipiscing; >elit")

    def test07(self):
        """Recognizes Full Command"""
        self.assertEqual(
            sh("echo 'shuf -i 1-10 | sort -n | tail -5' | ./tokenize"),
            "shuf\n-i\n1-10\n|\nsort\n-n\n|\ntail\n-5")

    def test08(self):
        """Recognizes Redir and pipe"""
        self.assertEqual(
            sh("echo 'sort < words.txt | nl' | ./tokenize"),
            "sort\n<\nwords.txt\n|\nnl")

if __name__ == '__main__':
    print(f"-= {YELLOW}Running tests for {TOKENIZE}{RESET} =-")
    unittest.main(testRunner = unittest.TextTestRunner(resultclass = PrettierTextTestResult))


