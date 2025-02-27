- Fixing 4040 using Pin of `Sequencials::Counter` that is made of 4 pins

```cpp
    ...
    case 15:
        return (m_counter.compute(pin));
    // m_counter only contains 4 pins
    // this produce an PinOutOfRangeException
```
