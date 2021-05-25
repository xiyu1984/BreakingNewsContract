import tensorflow as tf
import numpy as np

def tensorCalc():
    data = np.array([[1, 2, 3, 4]])
    tf.print(data)

    b = tf.keras.layers.Dense(4,activation = 'sigmoid', input_shape=(4, ))
    c = tf.keras.layers.Dense(2, activation = 'sigmoid', input_shape=(4,))

    @tf.function
    def myfunc():
        d = b(data)
        tf.print(d)
        tf.print(c(data))
        tf.print(c(d))

    myfunc()


tensorCalc()
