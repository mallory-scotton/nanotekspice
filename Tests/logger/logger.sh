rm -f log.bin

cat Tests/logger/epitech.txt | ./nanotekspice Circuits/logger.nts > /dev/null

echo "Log file contents (should be 'Ep1t3ch!!'):"
hexdump -C log.bin
cat log.bin
echo

# Compare with expected output
if [ "$(cat log.bin)" == "**Ep1t3ch!" ]; then
    echo "Test PASSED"
else
    echo "Test FAILED - Expected '**Ep1t3ch!' but got '$(cat log.bin)'"
fi
