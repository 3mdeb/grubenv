### grubenv edit module

[Ceedling](http://www.throwtheswitch.org/ceedling/) was used as test
environment.

How to test:

1. Install `ceedling`:

    ```
    gem install ceedling
    ```

2. Create new project

    ```
    ceedling new ceedlingProject
    ```

3. Create `grubenv` module

    ```
    rake module:create[grubenv]
    ```

    > Escape brackets in `zsh` shell

4. Clone this repository

5. Copy content of this repo into `ceedlingProject`:

    ```
    cp -r grubenv/* ../ceedlingProject/
    ```

6. Set `PREFIX` to your `grubenv/test` directory in `test_grubenv.c` file:

    ```
    #define PREFIX "/home/user/projects/test/grubenv/
    ```

7. Run tests:

    ```
    rake test:all
    ```

8. Test output:

    ```
    Test 'test_grubenv.c'
    ---------------------
    Running test_grubenv.out...

    -----------
    TEST OUTPUT
    -----------
    [test_grubenv.c]
      - "ERROR: src/grubenv.c:56: Ivalid grubenv file size: 981"
      - ""
      - "ERROR: src/grubenv.c:56: Ivalid grubenv file size: 1038"
      - ""
      - "ERROR: src/grubenv.c:44: Failed to open grubenv file: /home/...
      - ""
      - "ERROR: src/grubenv.c:79: Invalid grubenv header"
      - ""
      - ""

    --------------------
    OVERALL TEST SUMMARY
    --------------------
    TESTED:  19
    PASSED:  19
    FAILED:   0
    IGNORED:  0
    ```
