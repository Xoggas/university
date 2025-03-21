using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using NUnit.Framework;

namespace Tests;

public delegate int SortFunction(int[] arr, int len, int[] pos, int[] neg, ref int negCount);

[TestFixture]
public class Tests
{
    [SetUp]
    public void Setup()
    {
        var random = new Random();

        _arrayToSort = Enumerable.Repeat(0, 20).Select(_ => random.Next(-20, 20)).ToArray();
    }

    private int[] _arrayToSort;

    [Test]
    [TestCaseSource(nameof(SortTestCases))]
    public void DynamicLibraryFunctionTest((string name, SortFunction sortFunction) set)
    {
        var length = _arrayToSort.Length;
        var array = _arrayToSort.ToArray();
        var gotPosArray = new int[length];
        var gotNegArray = new int[length];
        var gotNegCount = 0;
        var gotPosLength = set.sortFunction(array, length, gotPosArray, gotNegArray, ref gotNegCount);

        var expectedPosArray = _arrayToSort.Where(x => x >= 0).OrderBy(x => x).ToArray();
        var expectedNegArray = _arrayToSort.Where(x => x < 0).OrderBy(x => x).ToArray();

        Assert.AreEqual(expectedPosArray.Length, gotPosLength);
        Assert.AreEqual(expectedNegArray.Length, gotNegCount);
        Assert.True(gotPosArray.Take(gotPosLength).SequenceEqual(expectedPosArray));
        Assert.True(gotNegArray.Take(gotNegCount).SequenceEqual(expectedNegArray));
    }

    [DllImport("lib1.dll", CallingConvention = CallingConvention.StdCall)]
    private static extern int Sort_stdcall_explicit(int[] arr, int len, int[] pos, int[] neg, ref int negCount);

    [DllImport("lib1.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int Sort_cdecl_explicit(int[] arr, int len, int[] pos, int[] neg, ref int negCount);

    [DllImport("lib1.dll", CallingConvention = CallingConvention.StdCall, EntryPoint = "_Sort_stdcall_implicit@20")]
    private static extern int Sort_stdcall_implicit(int[] arr, int len, int[] pos, int[] neg, ref int negCount);

    [DllImport("lib1.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int Sort_cdecl_implicit(int[] arr, int len, int[] pos, int[] neg, ref int negCount);
    
    private static IEnumerable<(string, SortFunction)> SortTestCases()
    {
        return SortFunctions().Select(x => (x.Method.Name, x));
    }

    private static IEnumerable<SortFunction> SortFunctions()
    {
        yield return Sort_stdcall_explicit;
        yield return Sort_cdecl_explicit;
        yield return Sort_stdcall_implicit;
        yield return Sort_cdecl_implicit;
    }
}