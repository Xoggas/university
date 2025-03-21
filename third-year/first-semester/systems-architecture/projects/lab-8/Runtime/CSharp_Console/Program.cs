using System.Diagnostics;

static void InsertionSort(int[] arr)
{
    for (var i = 1; i < arr.Length; i++)
    {
        var key = arr[i];
        var j = i - 1;

        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

var tests = new[] { 2500, 5000, 7500, 10000, 12500, 15000, 17500, 20000, 22500, 25000 };
var arr = File.ReadLines("test_data.txt").Select(int.Parse).ToArray();

foreach (var testSize in tests)
{
    var sw = Stopwatch.StartNew();

    var posRes = arr.Take(testSize).Where(x => x >= 0).ToArray();
    var negRes = arr.Take(testSize).Where(x => x < 0).ToArray();
    InsertionSort(posRes);
    InsertionSort(negRes);

    sw.Stop();

    Console.WriteLine($"Size: {testSize}, Total time: {sw.ElapsedMilliseconds} ms");
}